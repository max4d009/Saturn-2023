/*
 * page_speed.c
 *
 * Created: 31.07.2024 3:22:23
 *  Author: max4d
 */ 
#include "page_speed.h"

static uint8_t current_select = PAGE_SPEED_SELECT_MOTOR_SPEED;

void page_speed_render(uint8_t first_render)
{
	char str_buf[12];
	
	show_upm_anim();
	
	oled_printf(-1, 11, FONTID_6X8M, strcpy_P(str_buf, PSTR("%03d")), current.reel_speed_left);
	oled_printf(109, 11, FONTID_6X8M, strcpy_P(str_buf, PSTR("%03d")), current.reel_speed_right);
	
	oled_draw_menu_item(2, 37, FONTID_6X8M, current_select == PAGE_SPEED_SELECT_REEL_SIZE ? 1:0, strcpy_P(str_buf, PSTR("к:%d")), current.reel_size);
	oled_draw_menu_item(2, 46, FONTID_6X8M, current_select == PAGE_SPEED_SELECT_KINEMATICS_SPEED ? 1:0, strcpy_P(str_buf, PSTR("с.серв:%d")), current.kinematics_speed);
	oled_draw_menu_item(2, 55, FONTID_6X8M, current_select == PAGE_SPEED_SELECT_MOTOR_SPEED ? 1:0, strcpy_P(str_buf, PSTR("с.мот:%d")), current.motor_speed);
	
	oled_draw_line(117, 41, 118, 41);
	oled_draw_line(116, 45, 118, 45);
	oled_draw_line(117, 49, 118, 49);
 	
 	oled_draw_line(118, 26, 118, 64);
	
	uint8_t current_tension = 0;
	if ((current.tension - 5) >= 5) {
		current_tension = current.tension - 5;
	}
	
 	show_level_bar(8, 120, 5, current_tension, 64, 1);
	 
	oled_printf(94, 55, FONTID_6X8M, strcpy_P(str_buf, PSTR("нат:")));
}

void page_speed_menu() {}
	
void page_speed_select() 
{
	current_select++;
	if (current_select >= PAGE_SPEED_SELECT_NUM) {
		current_select = PAGE_SPEED_SELECT_REEL_SIZE;
	}
}
	
void page_speed_minus() {
		
	if (current_select == PAGE_SPEED_SELECT_MOTOR_SPEED) {
		if (current.motor_speed > 0) {
			i2c_set_motor_speed(current.motor_speed - 1, 0);
		}
	} else if (current_select == PAGE_SPEED_SELECT_KINEMATICS_SPEED) {
		if (current.kinematics_speed > 0) {
			current.kinematics_speed--;
			i2c_set_kinematics_speed(current.kinematics_speed);
		}
	} else if (current_select == PAGE_SPEED_SELECT_REEL_SIZE) {
		switch (current.reel_size) {
			case 18:
				current.reel_size = 15;
				i2c_set_reels_size(current.reel_size);
				break;
			case 15:
				current.reel_size = 13;
				i2c_set_reels_size(current.reel_size);
				break;				
		}
	}	
}
	
void page_speed_plus() 
{
	if (current_select == PAGE_SPEED_SELECT_MOTOR_SPEED) {
		if (current.motor_speed < 255) {
			uint8_t tmp = current.motor_speed + 1;
			if (tmp < 40) {
				tmp = 40;
			}
			//current.motor_speed = tmp;
			i2c_set_motor_speed(tmp, 0);
		}
	} else if (current_select == PAGE_SPEED_SELECT_KINEMATICS_SPEED) {
		if (current.kinematics_speed < 2) {
			current.kinematics_speed++;
			i2c_set_kinematics_speed(current.kinematics_speed);
		}
	} else if (current_select == PAGE_SPEED_SELECT_REEL_SIZE) {
		switch (current.reel_size) {
			case 13:
				current.reel_size = 15;
				i2c_set_reels_size(current.reel_size);
				break;
			case 15:
				current.reel_size = 18;
				i2c_set_reels_size(current.reel_size);
				break;
		}
	}
}

void page_speed_save() 
{
	if (current_select == PAGE_SPEED_SELECT_MOTOR_SPEED) {
		i2c_save_motor_speed();
	}
}
