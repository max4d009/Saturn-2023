/*
 * buttons_adc.h
 *
 * ? Created: 24.10.2023 18:27:32
 *  Author: m4d
 */ 
#ifndef BUTTONS_ADC_H_
#define BUTTONS_ADC_H_

#include "main.h"

void adc_buttons_init();
void keyboard_adc_timer();
void change_option(uint8_t option_);
void set_mode(uint8_t mode_);
void reset_rec_mode_inc();
void set_bbe_option(uint8_t value);

#endif /* BUTTONS_ADC_H_ */