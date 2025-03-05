/*
 * buttons_adc.c
 *
 * λ Created: 24.10.2023 18:27:15
 *  Author: m4d
 */ 
#include "buttons_adc.h"

static uint8_t adc_pressed = 0;
static uint8_t rec_mode_inc = 0;

void keyboard_adc_timer()
{
	uint16_t adc = adc_keyboard_read();
	
	static uint8_t little_inc = 0;
	static uint8_t wait_mode = 0;
		
	if (current.servo_real_mode != PLAY_MODE && current.servo_real_mode != REWIND_LITTLE_MODE && current.servo_real_mode != FORWARD_LITTLE_MODE) {
		little_inc = 0;
	}
	
	// Нажали и отпустили кнопку перемотки. При этом не держали долго до страбатывания функции "откат"
	if (adc > 1000 && little_inc != 0 && little_inc != 11 && wait_mode != 0) {
		set_mode(wait_mode);
		little_inc = 0;
		adc_pressed = 1;
		_delay_ms(100);
		return;
	}
	
	// Находились в режиме "откат", отпустили кнопку перемотки и включаем воспроизведение.
	if (adc > 1000 && little_inc == 11) {
		set_mode(PLAY_MODE);
		little_inc = 0;
		adc_pressed = 1;
		_delay_ms(100);
		return;
	}
	
	if (adc > 1000 || adc < 11) {
		adc_pressed = 0;
		return;
	}
	
	if (adc_pressed == 1) {
		return;
	}
	
	if (adc > 10 && adc < 30) {			 // REC
		
		if (current.servo_real_mode != STOP_MODE && is_rec_mode(current.servo_real_mode) == 0) {
			return;
		}
		
		if (current.servo_real_mode == REC_MODE_PLAY) {
			change_record_page();
			_delay_ms(100);
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
		change_option(EQ_OPTION);
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
			wait_mode = REWIND_MODE;
			if (little_inc < 10) {
				little_inc++;
				return;
			}
			if (little_inc == 10) {
				set_mode(REWIND_LITTLE_MODE);
				little_inc++;
				return;
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
			wait_mode = FORWARD_MODE;
			if (little_inc < 10) {
				little_inc++;
				return;
			}
			if (little_inc == 10) {
				set_mode(FORWARD_LITTLE_MODE);
				little_inc++;
				return;
			}
		} else {
			if (current.servo_real_mode == FORWARD_MODE) {
				set_mode(FORWARD_SEARCH_MODE);
			} else {
				set_mode(FORWARD_MODE);
			}
		}
	}
	
	if (adc > 10 && adc < 950) {
			adc_pressed = 1;
			_delay_ms(100);
	}

}

void change_option(uint8_t option_)
{
	switch (option_) {
		case NR_OPTION:
			current.nr++;
			if (current.nr > 1) {
				current.nr = 0;
			}
			i2c_send_option_motherboard(I2C_MOTHERBOARD_NR_OPTION, current.nr);
		break;
		
		case EQ_OPTION:
			current.eq++;
			if (current.eq > 2) {
				current.eq = 0;
			}
			i2c_send_option_motherboard(I2C_MOTHERBOARD_EQ_OPTION, current.eq);
		break;
		
		case KONTR_OPTION:
			current.kontr++;
			if (current.kontr > 1) {
				current.kontr = 0;
			}
			i2c_send_option_motherboard(I2C_MOTHERBOARD_KONTR_OPTION, current.kontr);
		break;
	}
}

void set_mode(uint8_t mode_)
{
	i2c_send_mode_motherboard(mode_);
	i2c_send_mode_servo(mode_);
}
