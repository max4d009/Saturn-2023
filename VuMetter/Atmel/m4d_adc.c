/*
 * m4d_adc.с
 *
 * Created: 02.12.2020 1:24:46
 *  Author: m4d
 */ 

#include "m4d_adc.h"

// Возвращает структуру с результами ацп
struct adc get_adc_result()
{
    return adc_result;
}

void m4d_adc_reset_result()
{
	adc_result.keyboard = 0;
	adc_result.count_measure = 2;
}

// Инициализация АЦП:
void m4d_adc_init_8(void) 
{
    m4d_adc_reset_result();
	
	// Включить АЦП
	ADCSRA |= (1 << ADEN);
	// Частота дискретизации 8
	ADCSRA |= (1 << ADPS0);
	ADCSRA |= (1 << ADPS1);
	ADCSRA &= ~(1 << ADPS2);
	
	ADMUX |= (1 << REFS0);
	ADMUX |= (1 << REFS1);
	
	// Разрешить прерывания
	ADCSRA |= (1 << ADIE);
}

// Считать показания с ADC
uint16_t m4d_adc_read()
{
	// Включаем АЦП
	ADCSRA |= (1 << ADSC);
	// Производим измерение
	while(ADCSRA & (1 << ADSC));
	return ADC;	
}

uint16_t m4d_adc_get_level(uint8_t channel) 
{
	uint16_t result = 0;
	uint16_t tmp;
	uint16_t tmp2 = 0;
	
	if(channel == 0){
		ADMUX &= ~(1 << MUX0);
	} else {
		ADMUX |= (1 << MUX0);
	}
	
	for (uint8_t i = 0; i<=2;i++) {
		tmp = m4d_adc_read();
		if (i > 0) {
			result = result + tmp;
			tmp2 = tmp2 + 1;
		}
	}
	if (tmp2 == 0) {
		return 0;
	}
	result = result / tmp2;
	
	return result;
}

ISR(ADC_vect)
{
	ADCSRA &= ~(1 << ADIF);
}