/*
 * i2c.c
 *
 *  Created: 17.12.2020 0:42:18
 *  Author: m4d
 */ 
#include "i2c.h"

static uint8_t i2c_start_condition();
static uint8_t i2c_stop_condition();
static uint8_t i2c_send_data(uint8_t *data, uint8_t long_wait, uint8_t count);
static uint8_t i2c_send_byte(uint8_t data);
static uint8_t TWIGetStatus(void);
static void after_i2c_read(uint8_t module_addr);
static void after_i2c_read_servo();
static void after_i2c_read_vu();
static int8_t get_query_id();
static void show_i2c_error();
static void i2c_send(uint8_t module, uint8_t type, uint8_t long_wait, uint8_t count_params,  ...);

static struct I2CReadByte i2c_read_byte_from_slave(uint8_t is_last);
//struct I2CReadByte i2c_byte_dto;

char debug_str_tmp[DEBUG_STR_LEFT_MAX_LEN];
static uint8_t i2c_stop = 0;
static struct I2CReadByte byte_dto;

void send_from_query_timer()
{
	for (uint8_t i = 0; i < I2C_SEND_QUERY_COUNT; i++) {
		if (query_list[i].sended == 0) {
			i2c_send_data(query_list[i].i2c_data, query_list[i].long_wait, query_list[i].count);
			query_list[i].sended = 1;
			
			//_delay_ms(10);
		}
	}
}

void i2c_init_as_master()
{
	for (uint8_t i = 0; i < I2C_SEND_QUERY_COUNT; i++) {
		query_list[i].sended = 1;
	}
	
	TWBR = 0x20;
 	TWCR |= (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
}	

// ФУНКЦИЯ ВОССТАНОВЛЕНИЯ ШИНЫ (добавьте в i2c.h)
void i2c_bus_recovery()
{
	// Отключаем I2C
	TWCR = 0;
	_delay_us(10);
	
	// Настраиваем SDA и SCL как выходы
	DDRC |= (1 << PC4) | (1 << PC5);  // SDA, SCL как выходы (проверьте номера пинов!)
	PORTC &= ~((1 << PC4) | (1 << PC5)); // Низкий уровень
	
	// Генерируем 9 тактов для освобождения шины
	for (uint8_t i = 0; i < 9; i++) {
		// SCL high
		PORTC |= (1 << PC5);
		_delay_us(5);
		
		// Проверяем SDA - если high, шина свободна
		if (PINC & (1 << PC4)) {
			break; // Шина свободна
		}
		
		// SCL low
		PORTC &= ~(1 << PC5);
		_delay_us(5);
	}
	
	// STOP condition
	PORTC |= (1 << PC4);  // SDA high
	_delay_us(5);
	PORTC |= (1 << PC5);  // SCL high
	_delay_us(5);
	PORTC &= ~(1 << PC5); // SCL low
	
	// Восстанавливаем настройки
	DDRC &= ~((1 << PC4) | (1 << PC5)); // Входы
	PORTC |= (1 << PC4) | (1 << PC5);   // Pull-up
	
	_delay_us(10);
	
	// Переинициализация I2C
	i2c_init_as_master();
	
	_delay_ms(1);
}

static uint8_t i2c_communication_attempt(uint8_t module_addr, uint8_t sla_w_module, uint8_t sla_r_module, uint8_t send_count)
{
	// 1. ПЕРВЫЙ START CONDITION
	if (i2c_start_condition() == 0) return 0;
	uint8_t status = TWIGetStatus();
	if (status != TW_START) { // TW_START
		i2c_stop_condition(); // Принудительный STOP
		return 0;
	}
	_delay_us(200); // Пауза после START
	
	
	
	// 2. ОТПРАВКА АДРЕСА ДЛЯ ЗАПИСИ (SLA+W)
	if (i2c_send_byte(sla_w_module) == 0) {
		i2c_stop_condition();
		return 0;
	}	
	status = TWIGetStatus();
	if (status != TW_MT_SLA_ACK) {
		i2c_stop_condition();
		return 0;
	}
	_delay_us(500); // ВАЖНАЯ ПАУЗА - дает время слейву подготовиться
	
	
	
	// 3. ПОВТОРНЫЙ START CONDITION
	if (i2c_start_condition() == 0) {
		i2c_stop_condition();
		return 0;
	}
	status = TWIGetStatus();
	if (status != 0x10 && status != 0x08) { // TW_REP_START (здесь возникает ошибка 16)
		i2c_stop_condition();
		return 0;
	}	
	_delay_us(200); // Пауза после повторного START
	
	
	
	// 4. ОТПРАВКА АДРЕСА ДЛЯ ЧТЕНИЯ (SLA+R)
	if (i2c_send_byte(sla_r_module) == 0) {
		i2c_stop_condition();
		return 0;
	}
	status = TWIGetStatus();
	if (status != 0x40) { // TW_MR_SLA_ACK (здесь возникает ошибка 72)
		i2c_stop_condition();
		return 0;
	}
	_delay_us(200); // Пауза перед чтением данных
	
	
	
	// 5. ЧТЕНИЕ ДАННЫХ ОТ СЛЕЙВА
	for (uint8_t i = 0; i < send_count - 1; i++) {
		byte_dto = i2c_read_byte_from_slave(0); // Чтение с ACK
		if (byte_dto.error > 0) {
			i2c_stop_condition();
			return 0;
		}
		status = TWIGetStatus();
		if (status != 0x50) { // TW_MR_DATA_ACK
			i2c_stop_condition();
			return 0;
		}		
		i2c_data[i] = byte_dto.byte;
		_delay_us(100); // Короткая пауза между байтами
	}
	
	// 6. ЧТЕНИЕ ПОСЛЕДНЕГО БАЙТА (с NACK)
	byte_dto = i2c_read_byte_from_slave(1); // Чтение с NACK
	if (byte_dto.error > 0) {
		i2c_stop_condition();
		return 0;
	}
	status = TWIGetStatus();
	if (status != 0x58) { // TW_MR_DATA_NACK
		i2c_stop_condition();
		return 0;
	}
	_delay_us(100);
	
	i2c_data[send_count - 1] = byte_dto.byte;
	
	// 7. STOP CONDITION
	if (i2c_stop_condition() == 0) {
		return 0;
	}

	_delay_us(100); // Пауза после STOP
	
	return 1; // УСПЕХ!
}


void i2c_timer(uint8_t module_addr, uint8_t sla_w_module, uint8_t sla_r_module)
{
	if (current.on == 1) {
		i2c_stop = 0;
	}
	
	if (i2c_stop == 1) {
		return;
	}
	
	uint8_t send_count = 0;
	
	if (module_addr == SERVO_ADDR) {
		if (current.debug == 0) {
			send_count = I2C_DATA_COUNT;
		} else if (current.debug == 1) {
			send_count = I2C_DATA_DEBUG_1_COUNT;
		} else if (current.debug == 2) {
			send_count = I2C_DATA_DEBUG_2_COUNT;
		}
	} else if (module_addr == VU_ADDR) {
		send_count = I2C_DATA_VU_COUNT;
	}
	
	uint8_t retry_count = I2C_MAX_RETRY;
	uint8_t success = 0;
	
	while (retry_count > 0 && !success) {
		success = i2c_communication_attempt(module_addr, sla_w_module, sla_r_module, send_count);
		    
		if (success == 0) {
			show_i2c_error();
			retry_count--;
			_delay_ms(5); // Короткая пауза между попытками
			    
			// Восстановление шины после нескольких ошибок
			if (retry_count == I2C_MAX_RETRY - 1) {
				i2c_bus_recovery();
			}
		}
	}
	    
	if (!success) {
		//show_i2c_error();
		i2c_stop = 1; // Остановить I2C при постоянных ошибках
	} else {
		after_i2c_read(module_addr);
		    
		if (current.on == 0) {
			i2c_stop = 1;
		}
	}
}

static void show_i2c_error()
{
	disp1color_FillScreenbuff(0);
	oled_printf(0,0, FONTID_6X8M, "i%d", TWIGetStatus());
	disp1color_UpdateFromBuff();
	_delay_ms(500);
}


// static void show_time_error()
// {
// 	disp1color_FillScreenbuff(0);
// 	oled_printf(0,0,FONTID_6X8M, "time", TWIGetStatus());
// 	disp1color_UpdateFromBuff();
// 	_delay_ms(500);
// }

static struct I2CReadByte i2c_read_byte_from_slave(uint8_t is_last)
{
	struct I2CReadByte i2c_byte_dto = {0, 0};
	
	if (is_last == 0) {
		TWCR = ((1<<TWINT)|(1<<TWEN)|(1<<TWEA));  // ACK
	} else {
		TWCR = ((1<<TWINT)|(1<<TWEN));  // NACK
	}
	
    uint16_t i2c_wait = I2C_TIMEOUT;
    while (!(TWCR & (1<<TWINT))) {
	    if (--i2c_wait == 0) {
		    i2c_byte_dto.error = 1;
		    TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWSTO);
		    return i2c_byte_dto;
	    }
	    _delay_us(1);
    }
	
	if (is_last == 0) {
		if ((TWSR & 0xF8) != TW_MR_DATA_ACK) { 
			i2c_byte_dto.error = 2;
			i2c_byte_dto.byte = 0;			
			TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWSTO);
			return i2c_byte_dto;
		}
	} else {
		if ((TWSR & 0xF8) != TW_MR_DATA_NACK) {
			i2c_byte_dto.error = 3;
			i2c_byte_dto.byte = 0;
			TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWSTO);
			return i2c_byte_dto;
		}
	}
	
	i2c_byte_dto.error = 0;
	i2c_byte_dto.byte = TWDR;
	
	_delay_us(500);
	return i2c_byte_dto;
}

static void after_i2c_read(uint8_t module_addr)
{
	if (module_addr == SERVO_ADDR) {
		after_i2c_read_servo();
	} else if (module_addr == VU_ADDR) {
		after_i2c_read_vu();
	}
}

static void after_i2c_read_vu()
{
	current.vu_peaks_mode = i2c_data[I2C_DATA_VU_PEAKS_MODE];
	current.vu_peaks_time = i2c_data[I2C_DATA_VU_PEAKS_TIME];
}

static void after_i2c_read_servo()
{
	uint16_t speed_sum = i2c_data[I2C_DATA_REEL_SPEED_LEFT] + i2c_data[I2C_DATA_REEL_SPEED_RIGHT];
	
	static uint8_t mute_delay = 0;
	if (mute_delay > 0) {
		mute_delay++;
		if (mute_delay == 40) {
			current.mute = 0;
			i2c_send_option_motherboard(I2C_MOTHERBOARD_MUTE_OPTION, current.mute, 0);
			mute_delay = 0;
		}
	}
	
	if (i2c_data[I2C_DATA_KINEMATICS_MODE_CURRENT] != current.servo_real_mode) {
		current.prev_mode = current.servo_real_mode;
		current.servo_real_mode = i2c_data[I2C_DATA_KINEMATICS_MODE_CURRENT];
		
		if (current.servo_real_mode == PLAY_MODE) {
			mute_delay = 1;
		}
	}
	
	current.reel_speed_left = i2c_data[I2C_DATA_REEL_SPEED_LEFT];
	current.reel_speed_right = i2c_data[I2C_DATA_REEL_SPEED_RIGHT];
	
	current.in_process = i2c_data[I2C_DATA_KINEMATICS_IN_PROCESS];
	current.repeat = i2c_data[I2C_DATA_REPEAT];
		
	if (current.reel_speed_left != 0 && current.reel_speed_right != 0) {
		if (current.servo_real_mode != REWIND_MODE) {
			
			if (current.prev_mode == REWIND_MODE && current.in_process == 1) {
				counterOut(speed_sum);
			} else {
				counterIn(speed_sum);
			}
			
			counterIn(speed_sum);
		} else {
			counterOut(speed_sum);
		}
	}
	
	if (current.debug == 0) {
		return;
	} else if (current.debug == 1) {
		audio_level.left = i2c_data[I2C_DATA_AUDIO_L];
		audio_level.right = i2c_data[I2C_DATA_AUDIO_R];
		audio_level.updated = 1;
			
		current.motor_speed = i2c_data[I2C_DATA_CONFIG_MOTOR_SPEED];
		current.tension = i2c_data[I2C_DATA_TENSION];
			
		debug_string[0] = '\0';
			
		for (uint8_t i = I2C_DATA_SERVO_DEBUG_FROM; i <= I2C_DATA_SERVO_DEBUG_TO; i++) {
			debug_string[i - I2C_DATA_SERVO_DEBUG_FROM] = i2c_data[i];
		}

		if (debug_string[0] == 'o' && debug_string[1] == 'k') {
			oled_show_info(debug_string);
			debug_string[0] = '\0';
		}
	} else if (current.debug == 2) {
		current.tension = i2c_data[I2C_DATA_TENSION];
		if (i2c_data[I2C_DATA_RESULT_EXIST] == 1) {
			oled_show_result(i2c_data[I2C_DATA_RESULT_VALUE]);
		}
	}
}

static int8_t get_query_id()
{
	for (uint8_t i = 0; i < I2C_SEND_QUERY_COUNT; i++) {
		if (query_list[i].sended == 1) {
			return i;
		}
	}
	
	return -1;
}

static void i2c_send(uint8_t module, uint8_t type, uint8_t long_wait, uint8_t count_params,  ...)
{
	
	int8_t query_id = get_query_id();	
	
	if (query_id == -1) {
		oled_show_info("OV");
		return;
	}
	//count_params = count_params+2;
	query_list[query_id].i2c_data[0] = module;
	query_list[query_id].i2c_data[1] = type;
	query_list[query_id].count = count_params+2;
	query_list[query_id].long_wait = long_wait;
	
	va_list param;
	va_start(param, count_params);
	for (uint8_t i = 0; i < count_params; i++) {
		query_list[query_id].i2c_data[i+2] = va_arg(param, int);		
	}
	va_end(param);
	query_list[query_id].sended = 0;
}

void i2c_send_config_current_servo(uint8_t servo_num)
{
	i2c_send(SLA_W_SERVO, I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_CURRENT_SERVO, 0, 1, servo_num);
}

void i2c_send_mode_servo(uint8_t _mode)
{	
	i2c_send(SLA_W_SERVO, I2C_SERVO_START_TRANSACTION_SYMBOL_MODE, 0, 1, _mode);
}

void i2c_send_repeat_servo(uint8_t repeat)
{
	i2c_send(SLA_W_SERVO, I2C_SERVO_START_TRANSACTION_SYMBOL_REPEAT, 0, 1, repeat);
}

void i2c_send_position_servo(uint8_t servo, uint8_t position)
{
	i2c_send(SLA_W_SERVO, I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_MOVE, 0, 2, servo, position);
}

void i2c_send_pid_koef(uint8_t pid_regulator_id, uint8_t koef_id, uint8_t value)
{
	i2c_send(SLA_W_SERVO, I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SEND_PID_KOEF, 0, 3, pid_regulator_id, koef_id, value);
}

void i2c_save_pid_koef(uint8_t pid_regulator_id, uint8_t koef_id, uint8_t value)
{
	i2c_send(SLA_W_SERVO, I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SAVE_PID_KOEF, 0, 3, pid_regulator_id, koef_id, value);
}

void i2c_send_config_current_pid(uint8_t pid_regulator_num)
{
	i2c_send(SLA_W_SERVO, I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_CURRENT_PID_REGULATOR, 0, 1, pid_regulator_num);
}

void i2c_set_motor_speed(uint8_t speed, uint8_t type)
{	
	i2c_send(SLA_W_SERVO, I2C_START_TRANSACTION_SYMBOL_SET_MOTOR_SPEED, 0, 2, speed, type);
}

void i2c_set_kinematics_speed(uint8_t speed)
{
	i2c_send(SLA_W_SERVO, I2C_SERVO_START_TRANSACTION_SYMBOL_SET_KINEMATICS_SPEED, 0, 1, speed);
}

void i2c_set_reels_size(uint8_t size)
{
	i2c_send(SLA_W_SERVO, I2C_SERVO_START_TRANSACTION_SYMBOL_SET_REELS_SIZE, 0, 1, size);
}

void i2c_save_motor_speed()
{
	i2c_send(SLA_W_SERVO, I2C_START_TRANSACTION_SYMBOL_SAVE_MOTOR_SPEED, 0, 0);
}

void i2c_save_position_servo(uint8_t servo, uint8_t parameter, uint8_t value)
{
	i2c_send(SLA_W_SERVO, I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SAVE_PARAM, 0, 3, servo, parameter, value);
}

void i2c_set_debug_mode(uint8_t debug)
{	
	i2c_send(SLA_W_SERVO, I2C_SERVO_START_TRANSACTION_SYMBOL_DEBUG_ENABLE, 0, 1, debug);
}

//
// SET FREQ. IF freq <= 20 freq*50 on gen board else *100
//
void i2c_send_freq_gen(uint8_t freq)
{
	i2c_send(SLA_W_GEN, I2C_GEN_START_TRANSACTION_SYMBOL_FREQ, 0, 1, freq);
}

void i2c_send_option_motherboard(uint8_t option,  uint8_t optionValue, uint8_t long_wait)
{
	i2c_send(SLA_W_MAINBOARD, I2C_MOTHERBOARD_START_TRANSACTION_SYMBOL_OPTION, long_wait, 2, option, optionValue);
}

void i2c_send_mode_motherboard(uint8_t mode)
{	
	i2c_send(SLA_W_MAINBOARD, I2C_MOTHERBOARD_START_TRANSACTION_SYMBOL_MODE, 0, 1, mode);
}

void i2c_send_vu_params(char transact_symbol, uint8_t value)
{	
	if (transact_symbol == I2C_VU_START_TRANSACTION_PEAKS_MODE_SAVE) {
		i2c_send(SLA_W_VU, transact_symbol, 1, 1, value);
	} else {
		i2c_send(SLA_W_VU, transact_symbol, 0, 1, value);
	}
}

static uint8_t i2c_send_data(uint8_t *data, uint8_t long_wait, uint8_t count)
{
	_delay_us(300);
	if (i2c_start_condition() == 0) {
		return 0;
	}
	
	_delay_us(300);

	uint8_t i = 0;
	do {
		if (i2c_send_byte(data[i]) == 0) {
			i2c_stop_condition();
			return 0;
		}
		
		uint8_t status = TWIGetStatus();
		
		if (i == 0 && status != 0x18) {
			i2c_stop_condition();
			return 0;
		} 
		
		if (i > 0 && status != 0x28) {
			i2c_stop_condition();
			return 0;
		}
		
		_delay_us(200);
		i++;
	} while (i < count);
	
	if (i2c_stop_condition() == 0) {
		return 0;
	}
	

	
	if (long_wait == 1) {
		_delay_ms(110); // надо избавиться на сервомодуле от ситуаций, когда в прерывании он долго думает
	} else {
		_delay_us(50);
	}
    
	return 1;
}

static uint8_t i2c_start_condition()
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    uint16_t timeout = I2C_TIMEOUT;
    while (!(TWCR & (1<<TWINT)) && timeout--) {
	    _delay_us(1);
    }
	
// 	if (timeout == 0) {
// 		show_time_error();
// 	}
    
    return (timeout > 0) ? 1 : 0;
}

static uint8_t i2c_send_byte(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	
    uint16_t timeout = I2C_TIMEOUT;
    while (!(TWCR & (1<<TWINT)) && timeout--) {
	    _delay_us(1);
    }
	
// 	if (timeout == 0) {
// 		show_time_error();
// 	}
	
	if (timeout == 0) return 0;
	
	// Проверка статуса (опционально, но рекомендуется)
	uint8_t status = TWIGetStatus();
	if (status != TW_MT_DATA_ACK && status != TW_MT_SLA_ACK && status != TW_MR_SLA_ACK) {
		return 0;
	}
    
    return 1;
}

static uint8_t i2c_stop_condition(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);

    uint16_t timeout = I2C_TIMEOUT;
    while ((TWCR & (1<<TWSTO)) && timeout--) {
	    _delay_us(1);
    }
	
	_delay_us(100);
	
// 	if (timeout == 0) {
// 		show_time_error();
// 	}
    
    return (timeout > 0) ? 1 : 0;
}

static uint8_t TWIGetStatus(void)
{
	uint8_t status;
	status = TWSR & 0xF8;
	return status;
}