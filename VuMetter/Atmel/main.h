/*
 * main.h
 *
 * Created: 12.07.2021 0:32:36
 *  Author: m4d
 */ 
#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <compat/twi.h>
#include <avr/io.h>

#include "m4d_adc.h"
#include "i2c.h"
#include "eeprom.h"

#define CLK PB2 // clock 14
#define DS PB5  // data 17
#define E PB4   // Enter 16
#define PORT PORTB
#define DDR DDRB

#define LEFT_CHANNEL 0
#define RIGHT_CHANNEL 1

void set_peaks_mode(uint8_t enable);
void set_peaks_time(uint8_t time);
uint8_t get_peaks_mode();
uint8_t get_peaks_time();

#endif /* MAIN_H_ */