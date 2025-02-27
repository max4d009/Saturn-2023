/*
 * buttons_adc.h
 *
 * Created: 24.10.2023 18:27:32
 *  Author: m4d
 */ 
#ifndef BUTTONS_ADC_H_
#define BUTTONS_ADC_H_

#include "main.h"

void keyboard_adc_timer();
void change_option(uint8_t option_);
void set_mode(uint8_t mode_);

#endif /* BUTTONS_ADC_H_ */