/*
 * m4d_i2c.h
 *
 * λ Created: 17.12.2020 1:03:28
 *  Author: m4d
 */ 

#ifndef M4D_I2C_H_
#define M4D_I2C_H_

#include "main.h"

void m4d_i2c_init_as_slave(void);
uint8_t execute_command_timer();

#define ME_ADDR 0x4A 
#define I2C_DATA_COUNT 2

#define I2C_GEN_START_TRANSACTION_SYMBOL_FREQ 1

#endif /* M4D_I2C_H_ */