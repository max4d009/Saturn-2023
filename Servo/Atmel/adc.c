/*
 * m4d_adc.с
 *
 * Created: 02.12.2020 1:24:46
 *  Author: m4d
 */ 
#include "adc.h"

static void adc_result_timer(uint8_t mux);
volatile uint8_t current_mux = 0;

// Инициализация АЦП:
void m4d_adc_init_8(void) 
{	
	adc_list[ADC_OPERATION_KEYBOARD].adc_result = 0;
	adc_list[ADC_OPERATION_KEYBOARD].status = ADC_STATUS_NO_COMPUTE;
	adc_list[ADC_OPERATION_KEYBOARD].repeat = 0;

	adc_list[ADC_OPERATION_LEFT_CHANNEL].adc_result = 0;
	adc_list[ADC_OPERATION_LEFT_CHANNEL].status = ADC_STATUS_NO_COMPUTE;
	adc_list[ADC_OPERATION_LEFT_CHANNEL].repeat = 0;

	adc_list[ADC_OPERATION_RIGHT_CHANNEL].adc_result = 0;
	adc_list[ADC_OPERATION_RIGHT_CHANNEL].status = ADC_STATUS_NO_COMPUTE;
	adc_list[ADC_OPERATION_RIGHT_CHANNEL].repeat = 0;

	adc_list[ADC_OPERATION_AUTO_STOP].adc_result = 0;
	adc_list[ADC_OPERATION_AUTO_STOP].status = ADC_STATUS_NO_COMPUTE;
	adc_list[ADC_OPERATION_AUTO_STOP].repeat = 0;

	adc_list[ADC_OPERATION_TENSION].adc_result = 0;
	adc_list[ADC_OPERATION_TENSION].status = ADC_STATUS_NO_COMPUTE;
	adc_list[ADC_OPERATION_TENSION].repeat = 0;

	adc_list[ADC_OPERATION_CURRENT_CONSUPTION].adc_result = 0;
	adc_list[ADC_OPERATION_CURRENT_CONSUPTION].status = ADC_STATUS_NO_COMPUTE;
	adc_list[ADC_OPERATION_CURRENT_CONSUPTION].repeat = 0;
	
	// Включить АЦП
	ADCSRA |=  (1 << ADEN);
	// Частота 250 000
	ADCSRA |= (1 << ADPS0);
	ADCSRA |= (1 << ADPS1);
	ADCSRA &= ~(1 << ADPS2);
		
	// Опорное напряжение по VCC 5v
	ADMUX |=  (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
		
	// Разрешить прерывания
	ADCSRA |= (1 << ADIE);
}

void compute_all_adc_timer()
{		
	if (adc_list[current_mux].repeat == 1 && (adc_list[current_mux].status == ADC_STATUS_NO_COMPUTE || adc_list[current_mux].status == ADC_STATUS_COMPUTED)) {
		adc_list[current_mux].status = ADC_STATUS_NEED_COMPUTE;
	} else if (adc_list[current_mux].status == ADC_STATUS_NO_COMPUTE || adc_list[current_mux].status == ADC_STATUS_COMPUTED) {
		current_mux++;
		if (current_mux >= ADC_OPERATION_COUNT) {
			current_mux = 0;
		}
	} else if (adc_list[current_mux].status == ADC_STATUS_NEED_COMPUTE) {
		mux_set(current_mux);
		ADCSRA |= (1 << ADSC);
		adc_list[current_mux].status = ADC_STATUS_IN_COMPUTE;
		adc_list[current_mux].adc_result = 0;		
	} else if (adc_list[current_mux].status == ADC_STATUS_IN_COMPUTE) { // Между попытками ждём	
	} else if (adc_list[current_mux].status == ADC_STATUS_ISR_COMPUTED) { // Если было получено значение в прерывании
		adc_result_timer(current_mux);
		adc_list[current_mux].status = ADC_STATUS_COMPUTED;
		current_mux++;
		if (current_mux >= ADC_OPERATION_COUNT) {
			current_mux = 0;
		}
	} 
}

void repeat_adc_on(uint8_t mux)
{
	adc_list[mux].repeat = 1;
}

void repeat_adc_off(uint8_t mux)
{
	adc_list[mux].repeat = 0;
	adc_list[mux].status = ADC_STATUS_NO_COMPUTE;
}

static void adc_result_timer(uint8_t mux)
{
	if (mux == ADC_OPERATION_LEFT_CHANNEL) {
		search_pause_with_adc(adc_list[mux].adc_result);
		audio_timer_left(adc_list[mux].adc_result);
	} else if (mux == ADC_OPERATION_RIGHT_CHANNEL) {	
		audio_timer_right(adc_list[mux].adc_result);
	} else if (mux == ADC_OPERATION_TENSION) {
		kinematics_mode.tension = adc_list[mux].adc_result;
		if (kinematics_mode.tension_sensor_enable == 1 && kinematics_mode.in_process == 0) {
			tension_sensor_set_timer();
		}
	} else if (mux == ADC_OPERATION_AUTO_STOP) {
		auto_stop_timer(adc_list[mux].adc_result);
	} else if (mux == ADC_OPERATION_CURRENT_CONSUPTION) {
		servo_consuption_observer_timer(adc_list[mux].adc_result);
	}
}

void mux_set(uint8_t mux)
{
	switch (mux) {
		case ADC_OPERATION_KEYBOARD:
		ADMUX &= ~(1 << MUX0);
		ADMUX &= ~(1 << MUX1);
		ADMUX &= ~(1 << MUX2);
		ADMUX &= ~(1 << MUX3);
		break;
	case ADC_OPERATION_LEFT_CHANNEL:
		ADMUX |=  (1 << MUX0);
		ADMUX |=  (1 << MUX1);
		ADMUX |=  (1 << MUX2);
		ADMUX &= ~(1 << MUX3);
		break;
	case ADC_OPERATION_RIGHT_CHANNEL:
		ADMUX &= ~(1 << MUX0);
		ADMUX |=  (1 << MUX1);
		ADMUX |=  (1 << MUX2);
		ADMUX &= ~(1 << MUX3);
		break;
	case ADC_OPERATION_AUTO_STOP:
		ADMUX |=  (1 << MUX0);
		ADMUX &= ~(1 << MUX1);
		ADMUX &= ~(1 << MUX2);
		ADMUX &= ~(1 << MUX3);
		break;
	case ADC_OPERATION_TENSION:
		ADMUX |=  (1 << MUX0);
		ADMUX |=  (1 << MUX1);
		ADMUX &= ~(1 << MUX2);
		ADMUX &= ~(1 << MUX3);
		break;
	case ADC_OPERATION_CURRENT_CONSUPTION:
		ADMUX &= ~(1 << MUX0);
		ADMUX |=  (1 << MUX1);
		ADMUX &= ~(1 << MUX2);
		ADMUX &= ~(1 << MUX3);
		break;
	}
}

ISR(ADC_vect)
{
	adc_list[current_mux].adc_result = ADC;
	adc_list[current_mux].status = ADC_STATUS_ISR_COMPUTED;
	ADCSRA &= ~(1 << ADIF);
}