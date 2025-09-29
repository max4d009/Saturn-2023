/*
 * buttons.c
 *
 * ? Created: 23.10.2023 20:21:19
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

// ????????? ??? ???????????? ??????
typedef struct {
	volatile uint8_t *DDR_REG;
	volatile uint8_t *PORT_REG;
	volatile uint8_t *PIN_REG;
	uint8_t pin;
	void (*operation)(void);
	uint8_t wait_period;
} ButtonConfig_t;

// ???????????? ?????? ? PROGMEM
static const ButtonConfig_t button_cfg[NUM_BUTTONS] PROGMEM = {
	{ &DDRD, &PORTD, &PIND, PD0, on,   10 }, // ON_BUTTON
	{ &DDRB, &PORTB, &PINB, PB7, off,  40 }, // OFF_BUTTON
	{ &DDRC, &PORTC, &PINC, PC0, menu, 2  }, // MENU_BUTTON
	{ &DDRC, &PORTC, &PINC, PC1, select, 2}, // SELECT_BUTTON
	{ &DDRC, &PORTC, &PINC, PC2, minus, 1 }, // MINUS_BUTTON
	{ &DDRC, &PORTC, &PINC, PC3, plus,  1 }, // PLUS_BUTTON
	{ &DDRD, &PORTD, &PIND, PD1, save,  2 }  // SAVE_BUTTON
};

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
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
	    ButtonConfig_t tmp;
	    memcpy_P(&tmp, &button_cfg[i], sizeof(ButtonConfig_t));

	    // ??????????? ???? ?? ????? ??? volatile
	    button_list[i].DDR_REG     = tmp.DDR_REG;
	    button_list[i].PORT_REG    = tmp.PORT_REG;
	    button_list[i].PIN_REG     = tmp.PIN_REG;
	    button_list[i].pin         = tmp.pin;
	    button_list[i].operation   = tmp.operation;
	    button_list[i].wait_period = tmp.wait_period;

	    // ??????????? ???? ? ?????????
	    *button_list[i].DDR_REG  &= ~(1 << button_list[i].pin);
	    *button_list[i].PORT_REG |=  (1 << button_list[i].pin);
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
	disp1color_SetBrightness(10);
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