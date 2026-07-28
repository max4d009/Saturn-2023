/*
 * buttons_adc.c
 *
 * ? Created: 24.10.2023 18:27:15
 *  Author: m4d
 */ 
#include "buttons_adc.h"

static uint8_t rec_mode_inc = 0;
static uint8_t button_pressed = 0;
static uint8_t block = 0;

void adc_buttons_init()
{
	// INT1: по спаду
	EICRA |= (1 << ISC10);  // ISC11=0, ISC10=1 -> любое изменение
	// Разрешаем прерываниe
	EIMSK |= (1 << INT1);
	// Сбрасываем флаг
	EIFR |= (1 << INTF1);
}

ISR (INT1_vect)
{
	button_pressed = 1;
}

void reset_rec_mode_inc()
{
	rec_mode_inc = 0;
}

static int8_t wait_mode = -1;
static int8_t wait_option = -1;
static uint8_t pressed_inc = 0;

static void keyboard_reset()
{
    wait_mode = -1;
	wait_option = -1;
	pressed_inc = 0;
	block = 2;
	button_pressed = 0;
}

void keyboard_adc_timer()
{
	if (block > 0) {
		block--;
		return;
	}
	
	if (button_pressed == 0) {
		return;
	}

	uint16_t adc = adc_keyboard_read();
	
	if (adc > 1000) {
		if (pressed_inc < 10) {
			if (wait_mode != -1) {
				set_mode(wait_mode);
			} else if (wait_option != -1) {
				change_option(wait_option);
			}
		}
		keyboard_reset();
		return;
	}

	if (adc > 10 && adc < 30) { // REC
		
		if (current.servo_real_mode != STOP_MODE && is_rec_mode(current.servo_real_mode) == 0) {
			keyboard_reset();
			return;
		}
		
		if (current.servo_real_mode == REC_MODE_PLAY) {
			change_record_page();
			keyboard_reset();
			return;
		}
		
		if (rec_mode_inc == 0) {
			set_mode(REC_MODE);
		} else if (rec_mode_inc == 1) {
			set_mode(REC_MODE_GEN);
		}
		
		set_record_page(rec_mode_inc);
		
		rec_mode_inc++;
		if (rec_mode_inc >= 3) {
			rec_mode_inc = 0;
		}
		
	} else if (adc > 90 && adc < 120) {  // KONTR
		change_option(KONTR_OPTION);
	} else if (adc > 150 && adc < 190) { // EQ
		if (pressed_inc == 0) {
			wait_option = EQ_OPTION;
			pressed_inc = 1;
			return;
		}
		if (pressed_inc < 10) {
			pressed_inc++;
			return;
		}
		static uint8_t prev_page = PAGE_OLED_TIMER;
		if (pressed_inc == 10) {
			pressed_inc = 11;
			if (current.page != PAGE_OLED_EQ) {
				prev_page = current.page;
				current.page = PAGE_OLED_EQ;
			} else {
				current.page = prev_page;
			}
		}
		
		//change_option(EQ_OPTION);
	} else if (adc > 270 && adc < 310) { // NR
		change_option(NR_OPTION);
	} else if (adc > 350 && adc < 405) { // REPEAT
		uint8_t repeat = current.repeat + 1;
		if (repeat > 3) {
			repeat = 0;
		}
		i2c_send_repeat_servo(repeat);
	} else if (adc > 480 && adc < 530) { // STOP
		rec_mode_inc = 0;
		set_mode(STOP_MODE);
		if (is_rec_mode(current.servo_real_mode)) {
			current.page = PAGE_OLED_TIMER;
		}
	} else if (adc > 660 && adc < 705) { // PAUSE
		set_mode(PAUSE_MODE);
	} else if (adc > 740 && adc < 780) { // REWIND
		if (current.servo_real_mode == PLAY_MODE) {
			if (pressed_inc < 10) {
				wait_mode = REWIND_MODE;
				pressed_inc++;
				return;
			}
			if (pressed_inc == 10) {
				set_mode(REWIND_LITTLE_MODE);
			}
		} else {
			if (current.servo_real_mode == REWIND_MODE) {
				set_mode(REWIND_SEARCH_MODE);
			} else {
				set_mode(REWIND_MODE);
			}
		}
	} else if (adc > 830 && adc < 870) { // PLAY
		if (is_rec_mode(current.servo_real_mode) || (current.servo_real_mode == PAUSE_MODE && is_rec_mode(current.prev_mode) )) {
			set_mode(REC_MODE_PLAY);
		} else {
			set_mode(PLAY_MODE);
		}
	} else if (adc > 900 && adc < 950) { // FORWARD
		if (current.servo_real_mode == PLAY_MODE) {
			if (pressed_inc < 10) {
				wait_mode = FORWARD_MODE;
				pressed_inc++;
				return;
			}
			if (pressed_inc == 10) {
				set_mode(FORWARD_LITTLE_MODE);
			}
		} else {
			if (current.servo_real_mode == FORWARD_MODE) {
				set_mode(FORWARD_SEARCH_MODE);
			} else {
				set_mode(FORWARD_MODE);
			}
		}
	}
	
	keyboard_reset();
}

void set_bbe_option(uint8_t value)
{	
	current.bbe = value;
	i2c_send_option_motherboard(I2C_MOTHERBOARD_BBE_OPTION, current.bbe, 0);
}

void change_option(uint8_t option_)
{
	switch (option_) {
		case NR_OPTION:
			current.nr++;
			if (current.nr > 1) {
				current.nr = 0;
			}
			i2c_send_option_motherboard(I2C_MOTHERBOARD_NR_OPTION, current.nr, 0);
		break;
		
		case EQ_OPTION:
			current.eq++;
			if (current.eq > 1) {
				current.eq = 0;
			}
			i2c_send_option_motherboard(I2C_MOTHERBOARD_EQ_OPTION, current.eq, 0);
		break;
		
		case BBE_OPTION:
			current.bbe++;
			if (current.bbe > 3) {
				current.bbe = 0;
			}
			i2c_send_option_motherboard(I2C_MOTHERBOARD_EQ_OPTION, current.bbe, 0);
		break;		
		
		case KONTR_OPTION:
			current.kontr++;
			if (current.kontr > 1) {
				current.kontr = 0;
			}
			i2c_send_option_motherboard(I2C_MOTHERBOARD_KONTR_OPTION, current.kontr, 0);
		break;
	}
}

void set_mode(uint8_t mode_)
{
	i2c_send_mode_motherboard(mode_);
	i2c_send_mode_servo(mode_);
}
