/*
 * adc.h
 *
 * Created: 08.12.2020 1:18:27
 *  Author: m4d
 */ 
#ifndef ADC_H_
#define ADC_H_

#include "main.h"

void adc_init_8(void);
uint16_t adc_keyboard_read();

#endif /* ADC_H_ */