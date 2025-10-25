/*
 * usart.h
 *
 * ? Created: 14.01.2021 15:41:20
 *  Author: m4d
 */ 
#ifndef USART_H_
#define USART_H_

#include "main.h"


#define BAUD_9600     103
#define BAUD_19200    51
#define BAUD_38400    25
#define BAUD_57600    16   // только если приемник терпит 2% ошибку

void USART_ini(unsigned int speed);
void usart_send_speed(uint16_t speed, uint8_t pid_type);
void usart_send_boost(uint8_t boost);


#endif /* USART_H_ */