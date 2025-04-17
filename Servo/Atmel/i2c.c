/*
 * m4d_i2c.c
 *
 * λ Created: 17.12.2020 0:42:18
 *  Author: m4d
 */ 
#include "i2c.h"

static void send_ack();
static void send_n_ack();

volatile uint8_t current_op = 0;
volatile uint8_t i2c_data[I2C_DATA_ALL_COUNT + 1];
volatile uint8_t i2c_data_current = 0;
volatile uint8_t debug_mode = 0;
volatile uint8_t config_servo = 0;
volatile uint8_t transactType = 0;
volatile uint8_t transactProcess = 0;
volatile uint8_t transactCounter = 0;
volatile uint8_t need_execute = 0;
volatile uint8_t config_pid = 0;

volatile unsigned char status;
volatile uint8_t transactData[10];


// Инициализация в роли ведомого устройства
void m4d_i2c_init_as_slave(void)
{
	memset(debug_string, '.', I2C_DEBUG_STR_MAX_LEN);

	TWAR = ME_ADDR & 0xFE;
	TWCR=(1<<TWEN)|(1<<TWEA)|(1<<TWINT)|(1 << TWIE);/* Enable TWI, Enable ack generation */
	while (!(TWCR & (1<<TWINT))); 
}

// Запускается по таймеру постоянно
// Если пришла команда по i2c выполнить еёё
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
			m4d_servo_init();
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_MOVE: // Пришла команда провернуть сервопривод во время настройки
			servo_list[(uint8_t)transactData[0]].need_angle = transactData[1];
		break;
		case I2C_START_TRANSACTION_SYMBOL_SET_MOTOR_SPEED: // Команда - установить скорость мотора в зависимости от типа настройки. Может придти число от 0 до 255.
			if (transactData[1] == 0) {
				kinematics_mode.motor_speed_play_correction = transactData[0]; // Установить скорость мотора при воспроизведении
				set_motor_speed(1000 + transactData[0], 1);
			} else if (transactData[1] == 3) {
				kinematics_mode.current_motor_speed = (uint8_t)transactData[0]*10; // Установить скорость мотора с типом пид регулятора 0
				set_motor_speed(transactData[0]*10, 0);
			} else {
				kinematics_mode.current_motor_speed = (uint8_t)transactData[0]*10; // Установить скорость мотора с типом пид регулятора 1
				set_motor_speed(transactData[0]*10, 1);
			}
		break;		
		case I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SAVE_PARAM: // Сохранить параметр сервопривода в eeprom
			update_servo_parameter_from_i2c(transactData[0], transactData[1], transactData[2]); 
			i2c_send_debug_int_var_oled("ok", transactData[2]);
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_DEBUG_ENABLE:  // Включить дебаг редим (больше данных передаётся по i2c в модуль дисплея)
			debug_mode = transactData[0];
			i2c_send_debug_int_var_oled("Debug", debug_mode);
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
			i2c_send_debug_int_var_oled("ok", kinematics_mode.reel_size);
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_CURRENT_PID_REGULATOR: // Приняли id пид регулятора который будем настраивать
			config_pid = transactData[0];
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SEND_PID_KOEF: // Приняли параметр пид регулятора
			switch (transactData[1]) {
				case CONFIG_PID_REGULATOR_P: 
					pid_regulator_list[transactData[0]].p = transactData[2];
					pid_regulator_calculated_list[transactData[0]].p = pid_regulator_list[transactData[0]].p * 0.01;
				break;
				case CONFIG_PID_REGULATOR_I:
					pid_regulator_list[transactData[0]].i = transactData[2];
					pid_regulator_calculated_list[transactData[0]].p = pid_regulator_list[transactData[0]].i * 0.01;
				break;
				case CONFIG_PID_REGULATOR_D:
					pid_regulator_list[transactData[0]].d = transactData[2];
					pid_regulator_calculated_list[transactData[0]].p = pid_regulator_list[transactData[0]].d * 0.01;					
				break;									
			}
			
		break;
		case I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SAVE_PID_KOEF: // Сохранили параметр пид регулятора
			update_pid_regulator_parameter_from_i2c(transactData[0], transactData[1], transactData[2]);
		break;		
	}
	need_execute = 0;
	transactType = 0;
	return 1;
}

// функция наполняем массив i2c_data данными, которые будут забраны дисплейным модулем при обращении на чтение
void update_i2c_data_timer()
{	
	i2c_data[I2C_DATA_REEL_SPEED_LEFT] = reels_speed.left_timer;
	i2c_data[I2C_DATA_REEL_SPEED_RIGHT] = reels_speed.right_timer;
	i2c_data[I2C_DATA_KINEMATICS_MODE_CURRENT] = kinematics_mode.current;
	i2c_data[I2C_DATA_KINEMATICS_IN_PROCESS] = kinematics_mode.in_process;
	i2c_data[I2C_DATA_REPEAT] = kinematics_mode.repeat;

	reels_speed.left_timer = 0;
	reels_speed.right_timer = 0;
	
	if (debug_mode == 1) { // при debug_mode 1 передаем больше параметров для отладки
		i2c_data[I2C_DATA_TENSION] = kinematics_mode.tension / 10;
		i2c_data[I2C_DATA_AUDIO_L] = audio_level.left;
		i2c_data[I2C_DATA_AUDIO_R] = audio_level.right;
		i2c_data[I2C_DATA_SERVO_PLAY_CURRENT_ANGLE] = servo_list[SERVO_PLAY].current_angle;
		i2c_data[I2C_DATA_SERVO_REWIND_CURRENT_ANGLE] = servo_list[SERVO_REWIND].current_angle;
		i2c_data[I2C_DATA_SERVO_LEFT_CURRENT_ANGLE] = servo_list[SERVO_LEFT].current_angle;
		i2c_data[I2C_DATA_SERVO_RIGHT_CURRENT_ANGLE] = servo_list[SERVO_RIGHT].current_angle;
		for (uint8_t i = I2C_DATA_SERVO_DEBUG_FROM; i <= I2C_DATA_SERVO_DEBUG_TO; i++) {
			i2c_data[i] = debug_string[i - I2C_DATA_SERVO_DEBUG_FROM];
		}
		if (debug_string[0] == 'o' && debug_string[1] == 'k') {
			memset(debug_string, '.', I2C_DEBUG_STR_MAX_LEN);
		}
		
		i2c_data[I2C_DATA_CONFIG_SERVO_MIN]     = servo_list[config_servo].min_angle;
		i2c_data[I2C_DATA_CONFIG_SERVO_MAX]     = servo_list[config_servo].max_angle;
		i2c_data[I2C_DATA_CONFIG_SERVO_PLAY]    = servo_list[config_servo].play_angle;
		i2c_data[I2C_DATA_CONFIG_SERVO_STOP]    = servo_list[config_servo].stop_angle;
		i2c_data[I2C_DATA_CONFIG_SERVO_FORWARD] = servo_list[config_servo].forward_angle;
		i2c_data[I2C_DATA_CONFIG_SERVO_REWIND]  = servo_list[config_servo].rewind_angle;
		i2c_data[I2C_DATA_CONFIG_SERVO_PAUSE]   = servo_list[config_servo].pause_angle;
		i2c_data[I2C_DATA_CONFIG_SERVO_SEARCH]  = servo_list[config_servo].search_angle;
		
		i2c_data[I2C_DATA_CONFIG_MOTOR_SPEED] = kinematics_mode.motor_speed_play_correction;
	} else if (debug_mode == 2) { // при debug_mode 2 сейчас пока отдаю настройки для пид регуляторов
		i2c_data[I2C_DATA_TENSION] = kinematics_mode.tension / 10;
		i2c_data[I2C_DATA_CONFIG_TENSION_P] = pid_regulator_list[config_pid].p;
		i2c_data[I2C_DATA_CONFIG_TENSION_I] = pid_regulator_list[config_pid].i;
		i2c_data[I2C_DATA_CONFIG_TENSION_D] = pid_regulator_list[config_pid].d;
	}
}

static void send_ack()
{
	TWCR = (1<<TWEN) | (1<<TWIE) | (1<<TWINT) | (1<<TWEA);
	//TWCR = (1<<TWEA | 1<<TWINT | 1<<TWEN);
}

static void send_n_ack()
{
	TWCR =  (1<<TWEN)|(1<<TWIE)|(1<<TWINT);
	//TWCR = (0<<TWEA | 1<<TWINT | 1<<TWEN);
}

ISR(TWI_vect)
{
	uint8_t send_count = 0;
	
	if (debug_mode == 0) {
		send_count = I2C_DATA_COUNT;
	} else if (debug_mode == 1) {
		send_count = I2C_DATA_DEBUG_1_COUNT;
	} else if (debug_mode == 2) {
		send_count = I2C_DATA_DEBUG_2_COUNT;
	}
	
	
	status = TWSR & 0xF8; // (11111000) Маска. Если настроили скорость.. но последние биты не являются нулями
	
	switch (status)
	{
		case 0x80:;  // Пришли данные от мастера. (До это мы уже приняли свой адрес)
		 	char data = TWDR;
		 	if (transactProcess == 0) {
		 		transactType = data;
		 		transactProcess = 1;
		 	} else {
		 		transactData[transactCounter] = data;
		 		transactCounter++;
		 	}
		 	send_ack();
		 	break;
		case 0xA0:;
			transactProcess = 0;
			transactCounter = 0;
			need_execute = 1;
			send_ack();
			break;			 
		//--- Status-Codes Slave Transmitter Modus
		case 0xa8: // own slave adress received
			update_i2c_data_timer();
			i2c_data_current = 0;
		case 0xb8: // byte was sent and ACK received

			TWDR = i2c_data[i2c_data_current];
			i2c_data_current++;
			if (i2c_data_current == send_count) {
				send_n_ack();
			} else {
				send_ack();
			}
		break;
		case 0xc0: // last byte was sent
		case 0xc8:
			//TWCR = 0xc5; // set TWEA
			send_ack();
		break;
		case 0x00: // BUS-Error
			TWCR = 0xd5;
		break;
		default:
			//send_ack();
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