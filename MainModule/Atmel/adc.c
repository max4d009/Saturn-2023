/*
 * adc.�
 *
 * Created: 02.12.2020 1:24:46
 *  Author: m4d
 */ 
#include <avr/interrupt.h>
#include "adc.h"

// ������������� ���:
void adc_init_8(void) 
{	
	// �������� ���
	ADCSRA |= (1 << ADEN);
	// ������� ������������� 8
	ADCSRA |= (1 << ADPS0);
	ADCSRA |= (1 << ADPS1);
	ADCSRA &= ~(1 << ADPS2);
	
	// ������� ���������� �� VCC 5v
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
}

// ��������� � ADC0
void admux_freq_level()
{
	ADMUX &= ~(1 << MUX0);
	ADMUX |= (1 << MUX1);
	ADMUX |= (1 << MUX2);
	ADMUX &= ~(1 << MUX3);
}

// ������� ��������� � ADC
uint16_t adc_read()
{
	// �������� ���
	ADCSRA |= (1 << ADSC);
	// ���������� ���������
	while(ADCSRA & (1 << ADSC));
	return ADC;	
}

// ������ ������ ��� ��� ����������
uint16_t adc_keyboard_read() 
{
    admux_freq_level();
 	uint16_t result = 0;
	uint8_t i;
	
	adc_read();
	
	for (i = 0; i < 5; i++) {
		result = result + adc_read();
	}

	result = result / 5;
	
	return result;
}