/*
 * main.h
 *
 * Created: 14.03.2021 15:32:42
 *  Author: m4d
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include <math.h>

#include "usart.h"
#include "m4d_eeprom.h"
//#include "m4d_i2c.h"

#define constrain_ard(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))


void set_target_speed(uint16_t speed, uint8_t saveInEeprom);
void set_p(uint8_t kp);
void set_i(uint8_t ki);
void set_d(uint8_t kd);
void set_u();
void set_target_speed_from_eeprom();
int get_target_speed();
float get_real_speed();
void set_fast_pid();
void set_slow_pid();
void set_brake_pid();
#endif /* MAIN_H_ */