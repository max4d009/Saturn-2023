/*
 * usart.c
 *
 * Created: 14.01.2021 15:41:00
 *  Author: m4d
 */ 
#include "usart.h"

char com_data[11];

int com_i = 0;
int debug = 0;

void USART_ini(unsigned int speed, int debug_)
{	
	DDRD |= (1 << 1); // Tx
	PORTD |= (1 << 1) | (1 << 0);
	UBRR0H = (unsigned char) (speed >> 8);
	UBRR0L = (unsigned char) speed;
	UCSR0A |= (1 << U2X0); // Удвоение частоты
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
	UCSR0B |= (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0);
	debug = debug_;
}

ISR(USART_RX_vect)
{
	char tmp_char = UDR0;
	
	if(tmp_char == '\r'){
		com_data[com_i] = 0;
		USART_send_string(com_data);
		process_transmit_data();
		com_i = 0;
		int i = 0;
		while (com_data[i]){
			com_data[i] = '_';
			i++;
		}
		return;
	}
	
	com_data[com_i] = tmp_char;
	com_i++;
}

// Обработка принятой через com - порт строки
void process_transmit_data()
{
// 	char value[8] = {0,0,0,0,0,0,0,0};
// 	int g=1;
// 	while(com_data[g]){
// 		value[g-1]=com_data[g];
// 		g++;
// 	}
	
//	switch (com_data[0]){

// 		case 's': ;
// 			int speed = atoi(value);
// 			set_target_speed(speed, 1);
// 			break;
// 			
// 		case 'p': ;
// 			float p = atof(value);
// 			set_p(p);
// 			break;	
// 			
// 		case 'i': ;
// 			float i = atof(value);
// 			set_i(i);
// 			break;
// 			
// 		case 'd': ;
// 			float d = atof(value);
// 			set_d(d);
// 			break;
// 			
// 		case 't': ;
// 			//float t = atof(value);
// 			//set_t(atof(value));
// 			break;	
// 			
// 		case 'u': ;
// 			set_u();
// 			break;											
//	}
}

void USART_Transmit(unsigned char data)
{
	// Начнем передавать данные, но только убедившись, что буфер пуст
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void USART_send_var(char *name, char *value)
{
	unsigned char i = 0;
	while(name[i]){
		USART_Transmit(name[i]);
		i = i + 1;
	}
	USART_Transmit (':');
	USART_Transmit (' ');
	i = 0;
	while(value[i]){
		USART_Transmit(value[i]);
		i = i + 1;
	}
	USART_Transmit (0x0d);
	USART_Transmit (0x0a);
}

void USART_send_string(char *string)
{
	unsigned char i = 0;
	while(string[i]){
		USART_Transmit(string[i]);
		i++;
	}
	USART_Transmit (0x0d);
	USART_Transmit (0x0a);
}

void IntToStr(long val, char str[])
{
	//~~~~~~~~~~~~~~~~~~~~~~~
	signed char i = 0, j = 0;
	unsigned char digits[5];
	//~~~~~~~~~~~~~~~~~~~~~~

	while (val)
	{
		digits[i] = val % 10; i++; val = val / 10;
	};
	i--;
	while ( i >= 0 )
	{
		str[j] = (digits[i]+ 0x30); j++; i--;
	};
	str[j] = 0x00;
}

// void USART_send_speed_graph(long speed, long pwm)
// {
// 	char speed_str[6], pwm_str[10];
// 	char *graph = "graph ";
// 	int i = 0;
// 	
// 	IntToStr(speed, speed_str);
// 	IntToStr(pwm, pwm_str);
// 	
// 	while(graph[i]){
// 		USART_Transmit(graph[i]);
// 		i = i + 1;
// 	}
// 	
// 	i = 0;
// 	while(speed_str[i]){
// 		USART_Transmit(speed_str[i]);
// 		i = i + 1;
// 	}
// 	
// 	USART_Transmit (' ');
// 	
// 	i = 0;
// 	while(pwm_str[i]){
// 		USART_Transmit(pwm_str[i]);
// 		i = i + 1;
// 	}
// 	USART_Transmit (0x0d);
// 	USART_Transmit (0x0a);
// }

void USART_send_int_var(char *name, int value)
{
	unsigned char i = 0;
	while(name[i]){
		USART_Transmit(name[i]);
		i = i + 1;
	}
	USART_Transmit (':');
	USART_Transmit (' ');
	char str[15];
	itoa(value, str, 10);
	
	i = 0;
	while(str[i]){
		USART_Transmit(str[i]);
		i = i + 1;
	}
		
	USART_Transmit (0x0d);
	USART_Transmit (0x0a);
}

void USART_send_float_var(char *name, float value)
{
	unsigned char i = 0;
	while(name[i]){
		USART_Transmit(name[i]);
		i = i + 1;
	}
	USART_Transmit (':');
	USART_Transmit (' ');
	char str[15];
	
	i = 0;
	while(str[i]){
		USART_Transmit(str[i]);
		i = i + 1;
	}
	
	USART_Transmit (0x0d);
	USART_Transmit (0x0a);
}

void USART_send_2_int_var(char *name, int value, char *name2, int value2)
{
	unsigned char i = 0;
	while(name[i]){
		USART_Transmit(name[i]);
		i = i + 1;
	}
	USART_Transmit ('/');
	i = 0;
	while(name2[i]){
		USART_Transmit(name2[i]);
		i = i + 1;
	}	
	
	USART_Transmit (':');
	USART_Transmit (' ');
	
	
	char str[15];
	char str2[15];
		
	i = 0;
	while(str[i]){
		USART_Transmit(str[i]);
		i = i + 1;
	}
	
	USART_Transmit ('/');
	
	i = 0;
	while(str2[i]){
		USART_Transmit(str2[i]);
		i = i + 1;
	}
	
	USART_Transmit (0x0d);
	USART_Transmit (0x0a);
}

