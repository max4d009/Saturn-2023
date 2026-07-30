/*
 * usart.c
 *
 * ? Created: 14.01.2021 15:41:00
 *  Author: m4d
 */ 
#include "usart.h"

static void USART_Transmit(char data);
static void USART_send_var(char name, char *value);

void USART_ini(unsigned int speed)
{
    // Настройка пинов
    DDRD |= (1 << 1);      // TX как выход
    DDRD &= ~(1 << 0);     // RX как вход
    
    // УБРАТЬ настройку PORTD - она не нужна и мешает!
    // PORTD |= (1 << 1) | (1 << 0);
    
    // Установка скорости
    UBRR0H = (unsigned char)(speed >> 8);
    UBRR0L = (unsigned char)speed;
    
	UCSR0A |= (1 << U2X0);
    
    // Настройка формата: 8N1
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    
    // Включение передатчика И приемника
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
}

char str_speed[10];

// Отправить скорость двигателя и тип пид регулятора в блок двигателя
void usart_send_speed(uint16_t speed, uint8_t pid_type)
{	
    // ЛОКАЛЬНЫЙ буфер - избегаем гонки данных
    char local_str_speed[10];
    itoa(speed, local_str_speed, 10);
		
    char var_name;
    switch (pid_type) {
	    case 1:  var_name = 's'; break;
	    case 2:  var_name = 'b'; break;
	    default: var_name = 'l'; break;
    }
    
    USART_send_var(var_name, local_str_speed);
}

// Deprecated
void usart_send_boost(uint8_t boost)
{
	itoa(boost, str_speed, 10);
	
	USART_send_var('b', str_speed);
}

static void USART_Transmit(char data)
{
	// Начнем передавать данные, но только убедившись, что буфер пуст
	while (!(UCSR0A & (1 << UDRE0)));
	
	// Очищаем флаги ошибок перед отправкой
	// UCSR0A |= (1 << TXC0);  // Сброс флага завершения передачи
	
	UDR0 = data;
	
    // Ждем полной отправки байта (опционально, для надежности)
    while (!(UCSR0A & (1 << TXC0)));
}

void USART_send_var(char name, char *value)
{
	uint8_t oldSREG = SREG;
	cli();
	USART_Transmit(name);

	uint8_t i = 0;
	while (value[i]) {
		USART_Transmit(value[i]);
		i++;
	}

	USART_Transmit(0x0d);
	SREG = oldSREG;
}