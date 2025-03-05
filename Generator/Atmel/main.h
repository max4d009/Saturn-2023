/*
 * main.h
 *
 * λ Created: 17.12.2020 2:05:42
 *  Author: m4d
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <compat/twi.h>
#include <avr/interrupt.h>

#include "m4d_i2c.h"
#include "ad9833.h"
#include "spi.h"

void set_freq(int freq);

#endif /* MAIN_H_ */