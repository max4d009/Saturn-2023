/*
 * m4d_i2c.h
 *
 * Created: 17.12.2020 1:03:28
 *  Author: m4d
 */ 


#ifndef M4D_I2C_H_
#define M4D_I2C_H_

#include "main.h"

void m4d_i2c_init_as_slave(void);
void m4d_i2c_init_as_master(void);
void m4d_i2c_send_debug_var_oled(char *varName, char *varValue);
void m4d_i2c_send_debug_int_var_oled(char *varName, int varValue);
int i2c_wait_twin();
void resetTWEN();

// #define RTC_ADDR_SERVO 0x4E // Адрес ведомого. Последний бит не является битом адреса. 7 бит адрес и последний флаг читать/писать
// #define SLA_R_SERVO RTC_ADDR_SERVO|0b00000001 // Макрос адреса + бит чтения
// #define SLA_W_SERVO RTC_ADDR_SERVO&0b11111110 // Макрос адреса + бит записи

#define ME_ADDR 0x1F // последний резрешение отклика на адрес


#define I2C_START_TRANSACTION_SYMBOL_SPEED '1'
#define I2C_START_TRANSACTION_SYMBOL_SPEED_SAVE '2'
#define I2C_START_TRANSACTION_SYMBOL_SPEED_FROM_EEPROM '3'
#define I2C_START_TRANSACTION_SYMBOL_P_SAVE '4'
#define I2C_START_TRANSACTION_SYMBOL_I_SAVE '5'
#define I2C_START_TRANSACTION_SYMBOL_D_SAVE '6'
#define I2C_STOP_TRANSACTION_SYMBOL '#'

#define I2C_OLED_START_TRANSACTION_SYMBOL_DEBUG '1'
#define I2C_OLED_STOP_TRANSACTION_SYMBOL '#'

#define OLED_ADDR 0x5F
#define SLA_R_OLED OLED_ADDR|0b00000001 // Макрос адреса + бит чтения
#define SLA_W_OLED OLED_ADDR&0b11111110 // Макрос адреса + бит записи

#define I2C_DEBUG_STR_MAX_LEN 21

#endif /* M4D_I2C_H_ */