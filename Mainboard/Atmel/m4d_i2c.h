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
uint8_t execute_command_timer();

#define ME_ADDR 0x5E // 94 // последний резрешение отклика на адрес

#define I2C_MOTHERBOARD_START_TRANSACTION_SYMBOL_MODE 1
#define I2C_MOTHERBOARD_START_TRANSACTION_SYMBOL_OPTION 2


#endif /* M4D_I2C_H_ */