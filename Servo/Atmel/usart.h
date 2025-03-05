/*
 * usart.h
 *
 * λ Created: 14.01.2021 15:41:20
 *  Author: m4d
 */ 
#ifndef USART_H_
#define USART_H_

#include "main.h"

void USART_ini(unsigned int speed);
void usart_send_speed(uint16_t speed, uint8_t pid_type);
void usart_send_boost(uint8_t boost);

#endif /* USART_H_ */