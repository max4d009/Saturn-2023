/*
 * m4d_eeprom.h
 *
 * λ Created: 17.12.2020 1:28:58
 *  Author: m4d
 */ 
#ifndef M4D_EEPROM_H_
#define M4D_EEPROM_H_

#include "main.h"

void EEPROM_write(uint8_t uiAddress, unsigned char ucData); //пишем
unsigned char EEPROM_read(uint8_t uiAddress); //читаем
void EEPROM_write_word(uint16_t uiAddress, uint16_t ucData); //пишем 2х-байтовую величину
uint16_t EEPROM_read_word(uint16_t uiAddress); //читаем 2х-байтовую величину

#define PR_MODE_SERVO_CONFIG_START 10
#define PR_MODE_MOTOR_SPEED 100
#define PR_MODE_PID_REGULATOR_CONFIG_START 200

void update_servo_parameter_from_i2c(uint8_t servo, uint8_t parameter, uint8_t value);
void update_pid_regulator_parameter_from_i2c(uint8_t pid_regulator, uint8_t parameter, uint8_t value);
uint8_t get_servo_eeprom_val(uint8_t servo, uint8_t parameter);
void eeprom_init();
uint8_t get_pid_regulator_eeprom_val(uint8_t pid_regulator, uint8_t parameter);

#endif /* M4D_EEPROM_H_ */