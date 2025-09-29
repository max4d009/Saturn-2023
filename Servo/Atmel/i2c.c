/*
 * m4d_i2c.c
 *
 * ? Created: 17.12.2020 0:42:18
 *  Author: m4d
 */ 
#include "i2c.h"

static void send_ack();
static void send_n_ack();

volatile uint8_t current_op = 0;
volatile uint8_t i2c_data_current = 0;
volatile uint8_t config_servo = 0;
volatile uint8_t transactType = 0;
volatile uint8_t transactProcess = 0;
volatile uint8_t transactCounter = 0;
volatile uint8_t need_execute = 0;
volatile uint8_t config_pid = 0;
volatile uint8_t result[2] = {0,0};
volatile unsigned char status;
volatile uint8_t transactData[10];

volatile uint8_t i2c_data_ready = 0;


volatile uint8_t i2c_data_buffer[2][I2C_DATA_ALL_COUNT];
volatile uint8_t active_buffer = 0;

void update_i2c_data_timer_background()
{
	if (i2c_data_ready == 1) {
		return;
	}
	
	
    static uint8_t write_buffer = 0;  // Буфер для записи (противоположный active_buffer)
	
    // Атомарно определяем буфер для записи
    uint8_t oldSREG = SREG;
    cli();
    write_buffer = 1 - active_buffer;
    SREG = oldSREG;
	
    // Обновляем данные в НЕактивном буфере (прерывания разрешены)
    i2c_data_buffer[write_buffer][I2C_DATA_KINEMATICS_MODE_CURRENT] = kinematics_mode.current;
    i2c_data_buffer[write_buffer][I2C_DATA_KINEMATICS_IN_PROCESS] = kinematics_mode.in_process;
    i2c_data_buffer[write_buffer][I2C_DATA_REPEAT] = kinematics_mode.repeat;
	
    if (kinematics_mode.debug_mode == 1) {
	    i2c_data_buffer[write_buffer][I2C_DATA_TENSION] = kinematics_mode.tension / 10;
	    i2c_data_buffer[write_buffer][I2C_DATA_AUDIO_L] = audio_level.left;
	    i2c_data_buffer[write_buffer][I2C_DATA_AUDIO_R] = audio_level.right;
	    i2c_data_buffer[write_buffer][I2C_DATA_SERVO_PLAY_CURRENT_ANGLE] = servo_list[SERVO_PLAY].current_angle;
	    i2c_data_buffer[write_buffer][I2C_DATA_SERVO_REWIND_CURRENT_ANGLE] = servo_list[SERVO_REWIND].current_angle;
	    i2c_data_buffer[write_buffer][I2C_DATA_SERVO_LEFT_CURRENT_ANGLE] = servo_list[SERVO_LEFT].current_angle;
	    i2c_data_buffer[write_buffer][I2C_DATA_SERVO_RIGHT_CURRENT_ANGLE] = servo_list[SERVO_RIGHT].current_angle;
	    
	    for (uint8_t i = I2C_DATA_SERVO_DEBUG_FROM; i <= I2C_DATA_SERVO_DEBUG_TO; i++) {
		    i2c_data_buffer[write_buffer][i] = debug_string[i - I2C_DATA_SERVO_DEBUG_FROM];
	    }
	    
	    if (debug_string[0] == 'o' && debug_string[1] == 'k') {
		   debug_string[0] = '\0';
	    }
	    
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_SERVO_MIN]     = servo_list[config_servo].min_angle;
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_SERVO_MAX]     = servo_list[config_servo].max_angle;
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_SERVO_PLAY]    = servo_list[config_servo].play_angle;
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_SERVO_STOP]    = servo_list[config_servo].stop_angle;
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_SERVO_FORWARD] = servo_list[config_servo].forward_angle;
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_SERVO_REWIND]  = servo_list[config_servo].rewind_angle;
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_SERVO_PAUSE]   = servo_list[config_servo].pause_angle;
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_SERVO_SEARCH]  = servo_list[config_servo].search_angle;
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_MOTOR_SPEED] = kinematics_mode.motor_speed_play_correction;
	} else if (kinematics_mode.debug_mode == 2) {
	    i2c_data_buffer[write_buffer][I2C_DATA_TENSION] = kinematics_mode.tension / 10;
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_TENSION_P] = pid_regulator_list[config_pid].p;
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_TENSION_I] = pid_regulator_list[config_pid].i;
	    i2c_data_buffer[write_buffer][I2C_DATA_CONFIG_TENSION_D] = pid_regulator_list[config_pid].d;
		i2c_data_buffer[write_buffer][I2C_DATA_RESULT_EXIST] = result[0];
		i2c_data_buffer[write_buffer][I2C_DATA_RESULT_VALUE] = result[1];
		result[0] = 0;
		result[1] = 1;
    }
	
    // Атомарное переключение
    oldSREG = SREG;
    cli();
    active_buffer = write_buffer;
    SREG = oldSREG;
	
	i2c_data_ready = 1;
}

// Инициализация в роли ведомого устройства
void m4d_i2c_init_as_slave(void)
{	
	// Инициализируем оба буфера
	for (uint8_t i = 0; i < I2C_DATA_ALL_COUNT; i++) {
		i2c_data_buffer[0][i] = 0;
		i2c_data_buffer[1][i] = 0;
	}
	
	memset(debug_string, '.', I2C_DEBUG_STR_MAX_LEN);

	TWAR = ME_ADDR & 0xFE;
	TWCR=(1<<TWEN)|(1<<TWEA)|(1<<TWINT)|(1 << TWIE);/* Enable TWI, Enable ack generation */
	while (!(TWCR & (1<<TWINT))); 
}

// Запускается по таймеру постоянно
// Если пришла команда по i2c выполнить её
uint8_t execute_command_timer()
{
	if (need_execute == 0) {
		return 0;
	}
	
	switch (transactType) {
		case I2C_SERVO_START_TRANSACTION_SYMBOL_MODE: // Приняли режим кинематики который нужно установить
			set_mode(transactData[0], 1, 0);
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_CURRENT_SERVO: // Приняли id сервопривода который будем настраивать
			config_servo = transactData[0];
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_MOVE: // Пришла команда провернуть сервопривод во время настройки
			servo_list[transactData[0]].need_angle = transactData[1];
		break;
		case I2C_START_TRANSACTION_SYMBOL_SET_MOTOR_SPEED: // Команда - установить скорость мотора в зависимости от типа настройки. Может придти число от 0 до 255.
			if (transactData[1] == 0) {
				kinematics_mode.motor_speed_play_correction = transactData[0]; // Установить скорость мотора при воспроизведении
				set_motor_speed(1000 + transactData[0], 1);
			} else if (transactData[1] == 3) {
				kinematics_mode.current_motor_speed = transactData[0]*10; // Установить скорость мотора с типом пид регулятора 0
				set_motor_speed(transactData[0]*10, 0);
			} else {
				kinematics_mode.current_motor_speed = transactData[0]*10; // Установить скорость мотора с типом пид регулятора 1
				set_motor_speed(transactData[0]*10, 1);
			}
		break;		
		case I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SAVE_PARAM: // Сохранить параметр сервопривода в eeprom
			update_servo_parameter_from_i2c(transactData[0], transactData[1], transactData[2]); 
			servo_position_load(transactData[0]);
			uint8_t savad_position = get_servo_eeprom_val_from_i2c(transactData[0], transactData[1]);
			servo_list[transactData[0]].need_angle = savad_position;
			i2c_send_debug_int_var_oled("ok", savad_position);
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_DEBUG_ENABLE:  // Включить дебаг режим (больше данных передаётся по i2c в модуль дисплея)
			kinematics_mode.debug_mode = transactData[0];
			i2c_send_debug_int_var_oled("Debug", kinematics_mode.debug_mode);
		break;
		case I2C_START_TRANSACTION_SYMBOL_SAVE_MOTOR_SPEED: // Сохранить скорость мотора при воспроизведении
			EEPROM_write(PR_MODE_MOTOR_SPEED, kinematics_mode.motor_speed_play_correction);
			i2c_send_debug_int_var_oled("ok", kinematics_mode.motor_speed_play_correction);
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_SET_KINEMATICS_SPEED: // Установить скорость работы кинематики
			kinematics_mode.kinematics_speed = transactData[0];
			i2c_send_debug_int_var_oled("ok", kinematics_mode.kinematics_speed);
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_REPEAT: // Установить режим повтора
			kinematics_mode.repeat = transactData[0];
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_SET_REELS_SIZE: // Установить размер катушек
			kinematics_mode.reel_size = transactData[0];
			if (kinematics_mode.current == PLAY_MODE) {
				if (kinematics_mode.reel_size == 15) {
					set_motor_speed((1000 + kinematics_mode.motor_speed_play_correction) / 2, 1);
				} else if (kinematics_mode.reel_size < 15) {
					set_motor_speed((1000 + kinematics_mode.motor_speed_play_correction) / 4, 1);
				} else {
					set_motor_speed(1000 + kinematics_mode.motor_speed_play_correction, 1);
				}
			}
			i2c_send_debug_int_var_oled("ok", kinematics_mode.reel_size);
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_CURRENT_PID_REGULATOR: // Приняли id пид регулятора который будем настраивать
			config_pid = transactData[0];
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SEND_PID_KOEF: // Приняли параметр пид регулятора
			switch (transactData[1]) {
				case I2C_DATA_CONFIG_TENSION_P: 
					pid_regulator_list[transactData[0]].p = transactData[2];
					pid_regulator_calculated_list[transactData[0]].p = pid_regulator_list[transactData[0]].p * 0.01;
				break;
				case I2C_DATA_CONFIG_TENSION_I:
					pid_regulator_list[transactData[0]].i = transactData[2];
					pid_regulator_calculated_list[transactData[0]].i = pid_regulator_list[transactData[0]].i * 0.01;
				break;
				case I2C_DATA_CONFIG_TENSION_D:
					pid_regulator_list[transactData[0]].d = transactData[2];
					pid_regulator_calculated_list[transactData[0]].d = pid_regulator_list[transactData[0]].d * 0.01;					
				break;									
			}
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SAVE_PID_KOEF: // Сохранили параметр пид регулятора
			update_pid_regulator_parameter_from_i2c(transactData[0], transactData[1], transactData[2]);
			result[0] = 1;
			result[1] = get_pid_regulator_eeprom_val_from_i2c(transactData[0], transactData[1]);
		break;		
	}
	
	need_execute = 0;
	transactType = 0;
	
	return 1;
}

// функция наполняет массив i2c_data данными, которые будут забраны дисплейным модулем при обращении на чтение
void update_i2c_data_timer()
{	
	// Обновляем скорости только в АКТИВНОМ буфере (текущие данные)
	uint8_t oldSREG = SREG;
	cli();
	uint8_t current_buf = active_buffer;
	SREG = oldSREG;
	
    i2c_data_buffer[current_buf][I2C_DATA_REEL_SPEED_LEFT] = reels_speed.left_timer;
    i2c_data_buffer[current_buf][I2C_DATA_REEL_SPEED_RIGHT] = reels_speed.right_timer;
	
	reels_speed.left_timer = 0;
	reels_speed.right_timer = 0;
}

static void send_ack()
{
	_delay_us(10);
	TWCR = (1<<TWEN) | (1<<TWIE) | (1<<TWINT) | (1<<TWEA);
}

static void send_n_ack()
{
	_delay_us(10);
	TWCR =  (1<<TWEN)|(1<<TWIE)|(1<<TWINT);
}

ISR(TWI_vect)
{
	uint8_t send_count = 0;
	
	if (kinematics_mode.debug_mode == 0) {
		send_count = I2C_DATA_COUNT;
	} else if (kinematics_mode.debug_mode == 1) {
		send_count = I2C_DATA_DEBUG_1_COUNT;
	} else if (kinematics_mode.debug_mode == 2) {
		send_count = I2C_DATA_DEBUG_2_COUNT;
	}
	
    // Защита от неожиданных состояний
    if ((TWSR & 0xF8) == 0xF8) {
	    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	    return;
    }
	
	status = TWSR & 0xF8; // (11111000) Маска. Если настроили скорость.. но последние биты не являются нулями
	
	// Проверяем, действительно ли обращаются к нам
	if (status == 0x60 || status == 0xA8) { // SLA+W или SLA+R
		uint8_t received_addr = TWDR >> 1; // Адрес из запроса
		if (received_addr != (TWAR >> 1)) {
			// Обращаются не к нам - игнорируем
			TWCR = (1<<TWINT)|(1<<TWEN);
			return;
		}
	}
	
	switch (status)
	{
		case 0x80:;  // Пришли данные от мастера.
		 	char data = TWDR;
		 	if (transactProcess == 0) {
		 		transactType = data;
		 		transactProcess = 1;
				transactCounter = 0;  // Сбрасываем счетчик
		 	} else {
				if (transactCounter < 10) {
					transactData[transactCounter] = data;
					transactCounter++;
				}
		 	}
		 	send_ack();
		 	break;
		case 0xA0:;  // STOP или REPEATED START получен
			
			transactProcess = 0;
			//transactCounter = 0;
			need_execute = 1;
			send_ack();
			break;			 
		//--- Status-Codes Slave Transmitter Modus
		case 0xA8: // Запрос на чтение (SLA+R received)
			update_i2c_data_timer();
			i2c_data_current = 0;
			
			TWDR = i2c_data_buffer[active_buffer][0];
			send_ack();
			break;
		case 0xB8: // byte was sent and ACK received
            i2c_data_current++;
            if (i2c_data_current < send_count) {
	            // Ещё есть данные для отправки
	            TWDR = i2c_data_buffer[active_buffer][i2c_data_current];
	            send_ack();
	         } else {
	            // Это был ПОСЛЕДНИЙ байт
				 TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWEA)|(1<<TWINT); // Последний байт - готовимся к завершению
	            //send_n_ack(); // Говорим мастеру "всё отправил"
	            // Данные НЕ обновляем - ждём STOP condition (case 0xA0)
				
            }
		break;
		case 0xC0: // Last byte sent, NACK received
		case 0xC8: // Last byte sent, ACK received  
            // ? Мастер подтвердил получение последнего байта
            // Но транзакция ещё не завершена - может быть повторный START
			    // Переходим в режим ожидания
			    TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWEA)|(1<<TWINT);
				i2c_data_ready = 0;
		break;
		case 0x00: // BUS-Error
			TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
		break;
		default:
			send_ack();
	}
}

void i2c_send_debug_int_var_oled(char *varName, uint16_t varValue)
{
	memset(debug_string, '\0', I2C_DEBUG_STR_MAX_LEN);
	char value_char[5] = {'\0'};
	itoa(varValue, value_char, 10);
	strcat(debug_string, varName);
	strcat(debug_string, ":");
	strcat(debug_string, value_char);
}