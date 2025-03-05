/*
 * buttons.c
 *
 * λ Created: 23.10.2023 20:21:19
 *  Author: m4d
 */ 
#include "buttons.h"

static void menu();
static void select();
static void minus();
static void plus();
static void save();

static uint8_t button_timer = 0;
static uint8_t button_timer_fast = 0;
static uint16_t off_timer_inc = 0;
static uint8_t vu_config_loaded = 0;

void buttons_timer()
{
	static uint8_t wait_count = 0;
	uint8_t any_key_pressed = 0;
	
	for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
		if (!(*button_list[i].PIN_REG & (1 << button_list[i].pin))) {
			any_key_pressed = 1;
			if (button_list[i].pressed == 1) {
				
				if (i == MINUS_BUTTON || i == PLUS_BUTTON) {
					if (button_timer_fast >= 10) {
						button_timer_fast++;
						if (button_timer_fast >= 20) {
							button_list[i].operation();
							button_timer_fast = 10;
						}
						continue;
					}
					button_timer++;
					if (button_timer > 30) {
						button_list[i].operation();
						button_timer = 1;
						button_timer_fast++;
					}
				}
				
				if (i == MENU_BUTTON) {
					if (button_timer < 70) {
						button_timer++;
						continue;
					}
					if (button_timer == 70) {
						button_timer = 71;
						current.page = PAGE_OLED_TIMER;
					}	
				}			
				continue;
			}
			button_timer = 0;
			button_timer_fast = 0;
			if (wait_count < button_list[i].wait_period) {
				wait_count++;
			}
			if (wait_count == button_list[i].wait_period) {
				button_list[i].operation();
				button_list[i].pressed = 1;
				wait_count = 0;
			}
		} else {
			button_list[i].pressed = 0;
		}
	}
	if (any_key_pressed == 0) {
		wait_count = 0;
	}
}

void buttons_init()
{
	button_list[ON_BUTTON].DDR_REG = &DDRD;
	button_list[ON_BUTTON].PORT_REG = &PORTD;
	button_list[ON_BUTTON].PIN_REG = &PIND;
	button_list[ON_BUTTON].pin = PD0;
	button_list[ON_BUTTON].operation = on;
	button_list[ON_BUTTON].wait_period = 10;
	
	button_list[OFF_BUTTON].DDR_REG = &DDRB;
	button_list[OFF_BUTTON].PORT_REG = &PORTB;
	button_list[OFF_BUTTON].PIN_REG = &PINB;
	button_list[OFF_BUTTON].pin = PB7;
	button_list[OFF_BUTTON].operation = off;
	button_list[OFF_BUTTON].wait_period = 40;
	
	button_list[MENU_BUTTON].DDR_REG = &DDRC;
	button_list[MENU_BUTTON].PORT_REG = &PORTC;
	button_list[MENU_BUTTON].PIN_REG = &PINC;
	button_list[MENU_BUTTON].pin = PC0;
	button_list[MENU_BUTTON].operation = menu;
	button_list[MENU_BUTTON].wait_period = 2;
	
	button_list[SELECT_BUTTON].DDR_REG = &DDRC;
	button_list[SELECT_BUTTON].PORT_REG = &PORTC;
	button_list[SELECT_BUTTON].PIN_REG = &PINC;
	button_list[SELECT_BUTTON].pin = PC1;
	button_list[SELECT_BUTTON].operation = select;
	button_list[SELECT_BUTTON].wait_period = 2;
	
	button_list[MINUS_BUTTON].DDR_REG = &DDRC;
	button_list[MINUS_BUTTON].PORT_REG = &PORTC;
	button_list[MINUS_BUTTON].PIN_REG = &PINC;
	button_list[MINUS_BUTTON].pin = PC2;
	button_list[MINUS_BUTTON].operation = minus;
	button_list[MINUS_BUTTON].wait_period = 1;
	
	button_list[PLUS_BUTTON].DDR_REG = &DDRC;
	button_list[PLUS_BUTTON].PORT_REG = &PORTC;
	button_list[PLUS_BUTTON].PIN_REG = &PINC;
	button_list[PLUS_BUTTON].pin = PC3;
	button_list[PLUS_BUTTON].operation = plus;
	button_list[PLUS_BUTTON].wait_period = 1;
	
	button_list[SAVE_BUTTON].DDR_REG = &DDRD;
	button_list[SAVE_BUTTON].PORT_REG = &PORTD;
	button_list[SAVE_BUTTON].PIN_REG = &PIND;
	button_list[SAVE_BUTTON].pin = PD1;
	button_list[SAVE_BUTTON].operation = save;
	button_list[SAVE_BUTTON].wait_period = 2;

	for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
		*button_list[i].DDR_REG  &= ~(1 << button_list[i].pin);
		*button_list[i].PORT_REG |= (1 << button_list[i].pin);
	}
}

void loading_anim(uint16_t wait_time)
{
	for (uint16_t i = 0; i < wait_time; i++) {
		show_loading();
	} 
}

void on()
{
	disp1color_Init();
	disp1color_SetBrightness(255);
	
	show_loading();
	i2c_init_as_master();
	show_loading();
	resetParameters();
	show_loading();
	PORTD |= (1 << STAND_BY_PIN);

	loading_anim(100);

	if (vu_config_loaded == 0) {
		for (uint8_t i = 0; i < 50;  i++) {
			i2c_timer(VU_ADDR, SLA_W_VU, SLA_R_VU);
			show_loading();
		}
		vu_config_loaded = 1;
	}
	current.page = PAGE_OLED_TIMER;

	current.on = 1;
}

void off_timer()
{
	if (off_timer_inc == 0) {
		return;
	}
	
	current.page = PAGE_OLED_OFF;

	
	if (off_timer_inc > 0) {
		off_timer_inc++;
	} 
	if (off_timer_inc == 5) {
		set_mode(STOP_MODE);
	} 
	if (off_timer_inc >= 10000) {
		current.on = 0;
		off_timer_inc = 0;
		disp1color_FillScreenbuff(0);
		disp1color_UpdateFromBuff();
		
		for (uint8_t i = 0; i < 255; i++) {
			i2c_timer(SERVO_ADDR, SLA_W_SERVO, SLA_R_SERVO);
		}
		TWCR &= ~(1 << TWEA); // Вспомнить зачем это..
		TWCR &= ~(1 << TWEN); // Вспомнить зачем это..
		
		PORTD &= ~(1 << STAND_BY_PIN);
		_delay_ms(1000);
		oled_sleep();
	}
}

void off()
{
	off_timer_inc = 1;
}

static void menu()
{	
	page_list[current.page].menu_click();
}

static void select()
{
	page_list[current.page].select_click();
}

static void minus()
{	
	page_list[current.page].minus_click();
}

static void plus()
{	
	page_list[current.page].plus_click();
}

static void save()
{
	page_list[current.page].save_click();
}