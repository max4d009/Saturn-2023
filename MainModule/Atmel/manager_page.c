/*
 * manager_page.c
 *
 * Created: 17.08.2024 1:28:08
 *  Author: max4d
 */ 
#include "manager_page.h"

static uint8_t rec_num_pages[3] = {
	PAGE_OLED_TIMER_REC,
	PAGE_OLED_FFT,
	PAGE_OLED_LEVEL
};

static uint8_t rec_page_inc = 0;

void manager_page_initPages()
{
	init_page_servo_config();
	init_page_tension_config();
	
	page_list[PAGE_OLED_TIMER].render_page = page_timer_render;
	page_list[PAGE_OLED_TIMER].menu_click = page_timer_menu;
	page_list[PAGE_OLED_TIMER].select_click = page_timer_select;
	page_list[PAGE_OLED_TIMER].minus_click = page_timer_minus;
	page_list[PAGE_OLED_TIMER].plus_click = page_timer_plus;
	page_list[PAGE_OLED_TIMER].save_click = page_timer_save;
		
	page_list[PAGE_OLED_FFT].render_page = page_fft_render;
	page_list[PAGE_OLED_FFT].menu_click = page_fft_menu;
	page_list[PAGE_OLED_FFT].select_click = page_fft_select;
	page_list[PAGE_OLED_FFT].minus_click = page_fft_minus;
	page_list[PAGE_OLED_FFT].plus_click = page_fft_plus;
	page_list[PAGE_OLED_FFT].save_click = page_fft_save;
		
	page_list[PAGE_OLED_SERVO_CONFIG].render_page = page_servo_config_render;
	page_list[PAGE_OLED_SERVO_CONFIG].menu_click = page_servo_config_menu;
	page_list[PAGE_OLED_SERVO_CONFIG].select_click = page_servo_config_select;
	page_list[PAGE_OLED_SERVO_CONFIG].minus_click = page_servo_config_minus;
	page_list[PAGE_OLED_SERVO_CONFIG].plus_click = page_servo_config_plus;
	page_list[PAGE_OLED_SERVO_CONFIG].save_click = page_servo_config_save;
		
	page_list[PAGE_OLED_IR_BINDING].render_page = page_ir_render;
	page_list[PAGE_OLED_IR_BINDING].menu_click = page_ir_menu;
	page_list[PAGE_OLED_IR_BINDING].select_click = page_ir_select;
	page_list[PAGE_OLED_IR_BINDING].minus_click = page_ir_minus;
	page_list[PAGE_OLED_IR_BINDING].plus_click = page_ir_plus;
	page_list[PAGE_OLED_IR_BINDING].save_click = page_ir_save;
		
	page_list[PAGE_OLED_LEVEL].render_page = page_level_render;
	page_list[PAGE_OLED_LEVEL].menu_click = page_level_menu;
	page_list[PAGE_OLED_LEVEL].select_click = page_level_select;
	page_list[PAGE_OLED_LEVEL].minus_click = page_level_minus;
	page_list[PAGE_OLED_LEVEL].plus_click = page_level_plus;
	page_list[PAGE_OLED_LEVEL].save_click = page_level_save;
		
	page_list[PAGE_OLED_SPEED].render_page = page_speed_render;
	page_list[PAGE_OLED_SPEED].menu_click = page_speed_menu;
	page_list[PAGE_OLED_SPEED].select_click = page_speed_select;
	page_list[PAGE_OLED_SPEED].minus_click = page_speed_minus;
	page_list[PAGE_OLED_SPEED].plus_click = page_speed_plus;
	page_list[PAGE_OLED_SPEED].save_click = page_speed_save;
		
	page_list[PAGE_OLED_TIMER_REC].render_page = page_timer_render;
	page_list[PAGE_OLED_TIMER_REC].menu_click = page_timer_menu;
	page_list[PAGE_OLED_TIMER_REC].select_click = page_timer_select;
	page_list[PAGE_OLED_TIMER_REC].minus_click = page_timer_minus;
	page_list[PAGE_OLED_TIMER_REC].plus_click = page_timer_plus;
	page_list[PAGE_OLED_TIMER_REC].save_click = page_timer_save;
		
	page_list[PAGE_OLED_OFF].render_page = page_off_render;
	page_list[PAGE_OLED_OFF].menu_click = page_off_menu;
	page_list[PAGE_OLED_OFF].select_click = page_off_select;
	page_list[PAGE_OLED_OFF].minus_click = page_off_minus;
	page_list[PAGE_OLED_OFF].plus_click = page_off_plus;
	page_list[PAGE_OLED_OFF].save_click = page_off_save;
	
	page_list[PAGE_OLED_MENU].render_page = page_menu_render;
	page_list[PAGE_OLED_MENU].menu_click = page_menu_menu;
	page_list[PAGE_OLED_MENU].select_click = page_menu_select;
	page_list[PAGE_OLED_MENU].minus_click = page_menu_minus;
	page_list[PAGE_OLED_MENU].plus_click = page_menu_plus;
	page_list[PAGE_OLED_MENU].save_click = page_menu_save;	
	
	page_list[PAGE_OLED_VU].render_page = page_vu_render;
	page_list[PAGE_OLED_VU].menu_click = page_vu_menu;
	page_list[PAGE_OLED_VU].select_click = page_vu_select;
	page_list[PAGE_OLED_VU].minus_click = page_vu_minus;
	page_list[PAGE_OLED_VU].plus_click = page_vu_plus;
	page_list[PAGE_OLED_VU].save_click = page_vu_save;
	
	page_list[PAGE_OLED_TENSION_CONFIG].render_page = page_tension_config_render;
	page_list[PAGE_OLED_TENSION_CONFIG].menu_click = page_tension_config_menu;
	page_list[PAGE_OLED_TENSION_CONFIG].select_click = page_tension_config_select;
	page_list[PAGE_OLED_TENSION_CONFIG].minus_click = page_tension_config_minus;
	page_list[PAGE_OLED_TENSION_CONFIG].plus_click = page_tension_config_plus;
	page_list[PAGE_OLED_TENSION_CONFIG].save_click = page_tension_config_save;
}

void manager_page_showPage()
{
	static uint8_t current_page = 255;
	
	disp1color_FillScreenbuff(0);
	
	if (current.page == PAGE_OLED_TENSION_CONFIG) {
		if (current.debug != 2) {
			current.debug = 2;
			i2c_set_debug_mode(2);
		}
	} else if (current.page != PAGE_OLED_TIMER && current.page != PAGE_OLED_TIMER_REC && current.page != PAGE_OLED_IR_BINDING && current.page != PAGE_OLED_OFF) {
		if (current.debug != 1) {
			current.debug = 1;
			i2c_set_debug_mode(1);
		}
	} else {
		if (current.debug != 0) {
			current.debug = 0;
			i2c_set_debug_mode(0);
		}
	}
	
	_delay_ms(20); // todo: Исследовать почему без задежки виснет
	
	if (current_page != current.page) {
		page_list[current.page].render_page(1);
		current_page = current.page;
	} else {
		page_list[current.page].render_page(0);
	}

 	disp1color_UpdateFromBuff();
}

void change_record_page()
{
	if (current.servo_real_mode == STOP_MODE) {
		rec_page_inc = 0;
	}
	current.page = rec_num_pages[rec_page_inc];
	rec_page_inc++;
	if (rec_page_inc >= REC_MENU_PAGES_NUM) {
		rec_page_inc = 0;
	}
}

void set_record_page(uint8_t rec_mode_inc)
{
	current.page = rec_num_pages[rec_mode_inc];
	rec_page_inc = rec_mode_inc;
}

void showConfigMenu(uint8_t num_params, struct ConfigParam *params, uint8_t current_param)
{
	char s1, s2;
	uint8_t x, y = 7, xx = 0, yy = 0;
	for (uint8_t i = 0; i < num_params; i++) {
		yy++; xx++;
		s1 = ' '; s2 = ' ';
		if (current_param == i) {
			s1 = '['; s2 = ']';
		}
		x = 0;
		if (xx == 2) {
			x = 64; xx = 0;
		}
		
		char param_name[10];
		strcpy_P(param_name, params[i].name);
		
		char str_buf[12];
		oled_printf(x, y, FONTID_6X8M, strcpy_P(str_buf, PSTR("%c%s:%03d%c")), s1, param_name, params[i].value, s2);
		if (yy == 2) {
			y = y + 8; yy = 0;
		}
	}
}