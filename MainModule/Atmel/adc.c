/*
 * adc.c
 *
 * ? Created: 02.12.2020 1:24:46
 *  Author: m4d
 */ 
#include <avr/interrupt.h>
#include "adc.h"

static uint16_t adc_read_median(uint8_t channel);
static uint16_t median_of_three(uint16_t a, uint16_t b, uint16_t c);

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
	uint16_t result = ADC;
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

// Чтение канала АЦП для клавиатуры
uint16_t adc_keyboard_read()
{
	return adc_read_median(6);
}

// Сравнение трех значений и выбор среднего
static uint16_t median_of_three(uint16_t a, uint16_t b, uint16_t c)
{
    if (a > b) { uint16_t t = a; a = b; b = t; }
    if (b > c) { uint16_t t = b; b = c; c = t; }
    if (a > b) { uint16_t t = a; a = b; b = t; }
    return b;
}

static uint16_t adc_read_median(uint8_t channel)
{
	adc_select_channel(channel);
	
	uint16_t a = adc_read();
	uint16_t b = adc_read();
	uint16_t c = adc_read();
	
	return median_of_three(a, b, c);
}