/*
 * main.c
 *
 *  Author: max4d
 */ 
#include "main.h"

static uint8_t timer_inc = 0;
static uint8_t buttons_timer_inc = 0;

void resetParameters()
{
	current.on = 0;
	current.servo_real_mode = STOP_MODE;
	current.prev_mode = STOP_MODE;
	current.repeat = 0;
	current.nr = 0;
	current.eq = 0;
	current.kontr = 1;
	current.uz_eq = 1;
	current.fix_level = 0;
	current.bias = 7;
	current.page = PAGE_OLED_TIMER;
	current.debug = 0;
	current.reel_speed_left = 0;
	current.reel_speed_right = 0;
	current.motor_speed = 0;
	current.kinematics_speed = 1;
	current.tension = 0;
	current.mute = 1;
	current.reel_size = 18;
	
	memset(debug_string, '.', DEBUG_STR_LEFT_MAX_LEN);
	timer_reset();
	_delay_ms(200);
	
	sei();
		
	timer_inc = 0;
	buttons_timer_inc = 0;
	
	i2c_send_option_motherboard(I2C_MOTHERBOARD_SADP_OPTION, current.bias);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_UZ_EQ_OPTION, current.uz_eq);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_EQ_OPTION, current.eq);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_NR_OPTION, current.nr);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_KONTR_OPTION, current.kontr);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_FIX_LEVEL_OPTION, current.fix_level);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_MUTE_OPTION, current.mute);
}

uint8_t is_rec_mode(uint8_t mode) 
{
	if (mode == REC_MODE || mode == REC_MODE_GEN || mode == REC_MODE_PLAY) {
		return 1;
	}
	
	return 0;
}

uint8_t is_rec_gen_mode()
{
	if (current.servo_real_mode == REC_MODE_GEN || (current.servo_real_mode == REC_MODE_PLAY && current.prev_mode == REC_MODE_GEN)) {
		return 1;
	}
	
	return 0;
}


int main(void)
{
	DDRD |= (1 << IR_LED_PIN);
	DDRD |= (1 << STAND_BY_PIN);
	DDRD |= (1 << REC_LED_PIN);
	DDRD &= ~(1 << IR_PIN);
	PORTD |= (1 << IR_PIN);
	
	disp1color_Init();
	_delay_ms(100);
	oled_sleep();
	
	manager_page_initPages();	
	adc_init_8();
	buttons_init();
	init_ir();
	current.vu_peaks_mode = 0;
	current.vu_peaks_time = 0;
	current.on = 0;
	
	while (1) {
		if (buttons_timer_inc > 20) {
			buttons_timer();
			buttons_timer_inc = 0;
		}
		buttons_timer_inc++;
		
		if (current.on == 0) {
			continue;
		}
			
		off_timer(0);
		
		if (timer_inc > 200) {	
			if (IR_impulse_count == 0) {
 				i2c_timer(SERVO_ADDR, SLA_W_SERVO, SLA_R_SERVO);
				send_from_query_timer();
 				keyboard_adc_timer();
				inc_counter();
				manager_page_showPage();
				debug_led();
			}
			timer_inc = 0;
		}
		timer_inc++;
	}
}