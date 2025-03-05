/*
 * m4d_eeprom.c
 *
 * Created: 17.12.2020 1:29:13
 *  Author: m4d
 */ 

#include "eeprom.h"

static uint8_t get_eeprom_addr_servo_parameter_by_i2c(uint8_t servo, uint8_t parameter);
static uint8_t get_eeprom_addr_pid_regulator_parameter_by_i2c(uint8_t pid_regulator, uint8_t parameter);

volatile uint8_t eeprom_addr_by_servo_param[NUM_SERVO][CONFIG_SERVO_COUNT_ALL] = {0};
volatile uint8_t eeprom_addr_by_pid_regulator_param[NUM_PID_REGULATOR][NUM_PID_REGULATOR_PARAM] = {0};

void eeprom_init()
{
	uint8_t eeprom_address = PR_MODE_SERVO_CONFIG_START;
	for (uint8_t servo = 0; servo < NUM_SERVO; servo++) {
		for (uint8_t param = 0; param < CONFIG_SERVO_COUNT_ALL; param++) {
			eeprom_addr_by_servo_param[servo][param] = eeprom_address;
			eeprom_address++;
		}
	}
	
	eeprom_address = PR_MODE_PID_REGULATOR_CONFIG_START;
	for (uint8_t pid_regulator = 0; pid_regulator < NUM_PID_REGULATOR; pid_regulator++) {
		for (uint8_t param = 0; param < NUM_PID_REGULATOR_PARAM; param++) {
			eeprom_addr_by_pid_regulator_param[pid_regulator][param] = eeprom_address;
			eeprom_address++;
		}
	}
}

void update_servo_parameter_from_i2c(uint8_t servo, uint8_t parameter, uint8_t value)
{
	uint8_t adr = get_eeprom_addr_servo_parameter_by_i2c(servo, parameter);
	EEPROM_write(adr, value);
	m4d_servo_init();
}

void update_pid_regulator_parameter_from_i2c(uint8_t pid_regulator, uint8_t parameter, uint8_t value)
{
	uint8_t adr = get_eeprom_addr_pid_regulator_parameter_by_i2c(pid_regulator, parameter);
	EEPROM_write(adr, value);
	tension_init();
}

uint8_t get_servo_eeprom_val(uint8_t servo, uint8_t parameter)
{
	uint8_t val = EEPROM_read(eeprom_addr_by_servo_param[servo][parameter]);
	if (parameter != CONFIG_SERVO_SPEED) {
		if (val < 40 || val > 254) {
			val = 150;
		}
	} else {
		if (val < 1 || val > 20) {
			val = 5;
		}
	}

	return val;
}

uint8_t get_pid_regulator_eeprom_val(uint8_t pid_regulator, uint8_t parameter)
{
	uint8_t val = EEPROM_read(eeprom_addr_by_pid_regulator_param[pid_regulator][parameter]);
	return val;
}

static uint8_t get_eeprom_addr_servo_parameter_by_i2c(uint8_t servo, uint8_t parameter)
{
	return eeprom_addr_by_servo_param[servo][parameter - I2C_DATA_CONFIG_SERVO_MIN];
}

static uint8_t get_eeprom_addr_pid_regulator_parameter_by_i2c(uint8_t pid_regulator, uint8_t parameter)
{
	return eeprom_addr_by_pid_regulator_param[pid_regulator][parameter - I2C_DATA_CONFIG_TENSION_P];
}

void EEPROM_write(uint8_t uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE)) //ждем освобождения флага окончания последней операцией с памятью
	{}
	EEAR = uiAddress; //Устанавливаем адрес
	EEDR = ucData; //Пишем данные в регистр
	EECR |= (1<<EEMPE); //Разрешаем запись
	EECR |= (1<<EEPE); //Пишем байт в память
}

unsigned char EEPROM_read(uint8_t uiAddress)
{
	while(EECR & (1<<EEPE))
	{} //ждем освобождения флага окончания последней операцией с памятью
	EEAR = uiAddress; //Устанавливаем адрес
	EECR |= (1<<EERE); //Запускаем операцию считывания из памяти в регистр данных
	return EEDR; //Возвращаем результат
}

void EEPROM_write_word(uint16_t uiAddress, uint16_t ucData)
{
	EEPROM_write(uiAddress, (unsigned char) ucData);
	unsigned char dt = ucData>>8;
	EEPROM_write(uiAddress+1, dt);
}

uint16_t EEPROM_read_word(uint16_t uiAddress)
{
	uint16_t dt = EEPROM_read(uiAddress+1)*256;
	asm("nop");
	dt += EEPROM_read(uiAddress);
	return dt;
}