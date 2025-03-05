/*
 * buttons.h
 *
 * ? Created: 23.10.2023 20:21:36
 *  Author: m4d
 */ 
#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "main.h"

#define ON_BUTTON_PIN PD0
#define OFF_BUTTON_PIN PB7
#define MENU_BUTTON_PIN PC0
#define SELECT_BUTTON_PIN PC1
#define MINUS_BUTTON_PIN PC2
#define PLUS_BUTTON_PIN PC3
#define SAVE_BUTTON_PIN PD1

#define ON_BUTTON 0
#define OFF_BUTTON 1
#define MENU_BUTTON 2
#define SELECT_BUTTON 3
#define MINUS_BUTTON 4
#define PLUS_BUTTON 5
#define SAVE_BUTTON 6

#define NUM_BUTTONS 7

void buttons_init();
void buttons_timer();
void on();
void off();
void off_timer();

struct Button {
	volatile unsigned char *PIN_REG;
	volatile unsigned char *PORT_REG;
	volatile unsigned char *DDR_REG;
	uint8_t pin;
	uint8_t wait_period;
	uint8_t pressed;
	void (*operation)();
};

volatile struct Button button_list[7];

#endif /* BUTTONS_H_ */