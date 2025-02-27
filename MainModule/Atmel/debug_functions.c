/*
 * debug_functions.c
 *
 * Created: 17.08.2024 1:35:11
 *  Author: max4d
 */ 
#include "debug_functions.h"

void debug_led()
{
	static uint8_t debug_led_enable = 0;
	
	if (current.debug == 1 && debug_led_enable == 0) {
		PORTD |= (1 << IR_LED_PIN);
		debug_led_enable = 1;
	} else if (current.debug == 0 && debug_led_enable == 1) {
		PORTD &= ~(1 << IR_LED_PIN);
		debug_led_enable = 0;
	}
}