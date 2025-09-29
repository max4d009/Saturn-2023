/*
 * adc.с
 *
 * ? Created: 02.12.2020 1:24:46
 *  Author: m4d
 */ 
#include <avr/interrupt.h>
#include "adc.h"

// Инициализация АЦП:
void adc_init_8(void) 
{	
	// Включить АЦП
	ADCSRA |= (1 << ADEN);
	
	// Делитель частоты АЦП = 64 > fADC = 8MHz / 64 = 125kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);
	ADCSRA &= ~(1 << ADPS0);
	
	// Опорное напряжение по VCC 5v
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
}

// Считать показания с АЦП
uint16_t adc_read()
{
	// Запуск преобразования
	ADCSRA |= (1 << ADSC);
	
	// Ждём завершения
	while(ADCSRA & (1 << ADSC));
	
	// Вернуть результат
	volatile uint16_t result = ADC; // volatile чтобы гарантировать чтение
	return result;
}

// Выбор канала АЦП (0–7)
void adc_select_channel(uint8_t channel)
{
	// Обнуляем биты MUX3:0, выбираем новый канал
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	
	// После переключения канала обычно делают одно «грязное» чтение
	adc_read();
}

// Усреднённое чтение с указанного канала
uint16_t adc_read_avg(uint8_t channel, uint8_t samples)
{
	adc_select_channel(channel);
	
	volatile uint32_t sum = 0;
	for (uint8_t i = 0; i < samples; i++) {
		sum += adc_read();
		//_delay_ms(1); // опционально, чтобы уменьшить шум
	}
	
	return (uint16_t)(sum / samples);
}

// Чтение канала АЦП для клавиатуры
uint16_t adc_keyboard_read()
{
	return adc_read_avg(6, 10);
}