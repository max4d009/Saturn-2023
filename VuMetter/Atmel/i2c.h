/*
 * i2c.h
 *
 * Created: 19.08.2024 1:32:45
 *  Author: max4d
 */ 
#ifndef I2C_H_
#define I2C_H_

#include "main.h"

#define ME_ADDR 0x3E // последний резрешение отклика на адрес

#define I2C_VU_START_TRANSACTION_PEAKS_MODE 1
#define I2C_VU_START_TRANSACTION_PEAKS_MODE_SAVE 2
#define I2C_VU_START_TRANSACTION_PEAKS_TIME 3
#define I2C_VU_START_TRANSACTION_PEAKS_TIME_SAVE 4

#define I2C_DATA_PEAKS_MODE 0
#define I2C_DATA_PEAKS_TIME 1
#define I2C_DATA_COUNT 2

void i2c_init_as_slave(void);
uint8_t execute_command_timer();

#endif /* I2C_H_ */