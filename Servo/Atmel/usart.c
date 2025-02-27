/*
 * λ usart.c
 *
 * Created: 14.01.2021 15:41:00
 *  Author: m4d
 */ 
#include "usart.h"

static void USART_Transmit(char data);
static void USART_send_var(char name, char *value);

void USART_ini(unsigned int speed)
{
	DDRD |= (1 << 1);
	PORTD |= (1 << 1) | (1 << 0);
	UBRR0H = (unsigned char) (speed >> 8);
	UBRR0L = (unsigned char) speed;
	UCSR0A |= (1 << U2X0);
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
	UCSR0B |= (1 << TXEN0);
}

char str_speed[10];

void usart_send_speed(uint16_t speed, uint8_t pid_type)
{	
	itoa(speed, str_speed, 10);
		
	if (pid_type == 1) {
		USART_send_var('s', str_speed);
	} else if (pid_type == 2) {
		USART_send_var('b', str_speed);
	} else {
		USART_send_var('l', str_speed);
	}
	
	
}

void usart_send_boost(uint8_t boost)
{
	itoa(boost, str_speed, 10);
	
	USART_send_var('b', str_speed);
}

static void USART_Transmit(char data)
{
	// Начнем передавать данные, но только убедившись, что буфер пуст
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
	_delay_us(200);
}

void USART_send_var(char name, char *value)
{
	USART_Transmit(name);

	uint8_t i = 0;
	while (value[i]) {
		USART_Transmit(value[i]);
		i++;
	}

	USART_Transmit(0x0d);
}