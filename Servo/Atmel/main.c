/*
 * Real-to-real tape recorder main.c
 *
 * λ Created: 08.04.2019 2:01:38
 * Author : m4d
 */ 
#include "main.h"

int main(void)
{
	_delay_ms(200);
	m4d_i2c_init_as_slave();
	sei(); 
	eeprom_init();
 	m4d_adc_init_8();
	USART_ini(8);
    m4d_servo_init();
	tension_init();
	audio_init();

    // Включаем каналы АЦП на постоянную цикличную работу. 
	repeat_adc_on(ADC_OPERATION_LEFT_CHANNEL); // измерить уровень по левому каналу
	repeat_adc_on(ADC_OPERATION_RIGHT_CHANNEL); // измерить уровень по правому каналу
	repeat_adc_on(ADC_OPERATION_TENSION); // измерить уровень по датчикку натяжения
	repeat_adc_on(ADC_OPERATION_CURRENT_CONSUPTION);  // измерить уровень потребулению тока сервоприводами
	repeat_adc_on(ADC_OPERATION_AUTO_STOP);  // измерить уровень по каналу автостопа
	
	DDRD &= ~(1 << PD4);
	PORTD &= ~(1 << PD4);
	
    while (1) {	
    }
}