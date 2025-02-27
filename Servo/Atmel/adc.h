/*
 * λ m4d_adc.h
 *
 * Created: 08.12.2020 1:18:27
 *  Author: m4d
 */ 
#ifndef M4D_ADC_H_
#define M4D_ADC_H_

#include "main.h"

#define ADC_OPERATION_KEYBOARD 0
#define ADC_OPERATION_LEFT_CHANNEL 1
#define ADC_OPERATION_RIGHT_CHANNEL 2
#define ADC_OPERATION_AUTO_STOP 3
#define ADC_OPERATION_TENSION 4
#define ADC_OPERATION_CURRENT_CONSUPTION 5

#define ADC_OPERATION_COUNT 6

#define ADC_STATUS_NO_COMPUTE 0
#define ADC_STATUS_NEED_COMPUTE 1
#define ADC_STATUS_IN_COMPUTE 2
#define ADC_STATUS_ISR_COMPUTED 3
#define ADC_STATUS_COMPUTED 4

#define ADC_RETURN_NULL 2000

void m4d_adc_init_8(void);
void compute_all_adc_timer();
void mux_set(uint8_t mux);
uint16_t m4d_adc_read(uint8_t with_return);

struct adcResult {
	volatile uint16_t adc_result;
	volatile uint8_t status;
	volatile uint8_t repeat;
};

volatile struct adcResult adc_list[ADC_OPERATION_COUNT];

void repeat_adc_on(uint8_t mux);
void repeat_adc_off(uint8_t mux);

#endif /* M4D_ADC_H_ */