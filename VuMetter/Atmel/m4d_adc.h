/*
 * m4d_adc.h
 *
 * Created: 08.12.2020 1:18:27
 *  Author: m4d
 */ 
#ifndef M4D_ADC_H_
#define M4D_ADC_H_

#include "main.h"

struct adc {
	uint16_t keyboard;
	uint16_t tension_middle;
	uint16_t tension_hight;
	char tension_middle_complete;
	char tension_hight_complete;
	char count_measure;
};

struct adc adc_result;


void m4d_adc_init_8(void);
uint16_t m4d_adc_get_level(uint8_t channel);
struct adc get_adc_result();
void m4d_adc_reset_result();
#endif /* M4D_ADC_H_ */