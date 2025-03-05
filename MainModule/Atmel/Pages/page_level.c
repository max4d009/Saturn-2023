/*
 * pageLevel.c
 *
 * λ Created: 19.07.2024 22:04:24
 *  Author: max4d
 */ 
#include "page_level.h"

static void show_freq();
static void show_level_bars(uint8_t x, uint8_t y);
static void show_menu(uint8_t x, uint8_t y);

static uint8_t current_freq = 30;
static uint8_t current_select = SELECT_PAGE_LEVEL_FIX_LEVEL;

void page_level_render(uint8_t first_render)
{
	if (first_render == 1) {
		current_select = SELECT_PAGE_LEVEL_FIX_LEVEL;
	}
	
	if (first_render == 1 && is_rec_gen_mode() == 1) {
		i2c_send_freq_gen(current_freq);
	}
	
	char str_buf[10];
	oled_printf(2, 1, FONTID_6X8M, strcpy_P(str_buf, PSTR("Уровень")));
 	show_menu(2, 10);
	 
	if (is_rec_gen_mode() == 1) {
		show_freq();
	}
 	
 	show_level_bars(69, 11);
}

void page_level_menu()
{
	//current.page = PAGE_OLED_TIMER;
}
	
void page_level_select()
{	
	current_select++;
	if (is_rec_gen_mode() == 1) {
		if (current_select >= SELECT_PAGE_LEVEL_NUM_REC) {
			current_select = SELECT_PAGE_LEVEL_FIX_LEVEL;
		}
	} else {
		if (current_select >= SELECT_PAGE_LEVEL_NUM) {
			current_select = SELECT_PAGE_LEVEL_FIX_LEVEL;
		}
	}
}

void page_level_minus()
{
	if (current_select == SELECT_PAGE_LEVEL_FREQ) {
		if (current_freq <= 20 && current_freq > 0) {
			current_freq--;
		} else if (current_freq > 20) {
			current_freq = current_freq - 10;
		}
		if (current_freq < 1) {
			current_freq = 240;
		}
		i2c_send_freq_gen(current_freq);
	} else if (current_select == SELECT_PAGE_LEVEL_BIAS) {
		if (current.bias > 0) {
			current.bias--;
			i2c_send_option_motherboard(I2C_MOTHERBOARD_SADP_OPTION, current.bias);
		}
	} else if (current_select == SELECT_PAGE_LEVEL_UZ_EQ) {
		if (current.uz_eq > 0) {
			current.uz_eq--;
			i2c_send_option_motherboard(I2C_MOTHERBOARD_UZ_EQ_OPTION, current.uz_eq);
		}
	} else if (current_select == SELECT_PAGE_LEVEL_FIX_LEVEL) {
		current.fix_level = 0;
		i2c_send_option_motherboard(I2C_MOTHERBOARD_FIX_LEVEL_OPTION, current.fix_level);
	} else if (current_select == SELECT_PAGE_LEVEL_MUTE) {
		current.mute = 0;
		i2c_send_option_motherboard(I2C_MOTHERBOARD_MUTE_OPTION, current.mute);
	}
}

void page_level_plus()
{
	if (current_select == SELECT_PAGE_LEVEL_FREQ) {
		if (current_freq < 20) {
			current_freq++;
		} else if (current_freq >= 20) {
			current_freq = current_freq + 10;
		}
		if (current_freq > 240) {
			current_freq = 1;
		}
		i2c_send_freq_gen(current_freq);
	} else if (current_select == SELECT_PAGE_LEVEL_BIAS) {
		if (current.bias < 15) {
			current.bias++;
			i2c_send_option_motherboard(I2C_MOTHERBOARD_SADP_OPTION, current.bias);
		}
	} else if (current_select == SELECT_PAGE_LEVEL_UZ_EQ) {
		if (current.uz_eq < 3) {
			current.uz_eq++;
			i2c_send_option_motherboard(I2C_MOTHERBOARD_UZ_EQ_OPTION, current.uz_eq);
		}
	} else if (current_select == SELECT_PAGE_LEVEL_FIX_LEVEL) {
		current.fix_level = 1;
		i2c_send_option_motherboard(I2C_MOTHERBOARD_FIX_LEVEL_OPTION, current.fix_level);
	} else if (current_select == SELECT_PAGE_LEVEL_MUTE) {
		current.mute = 1;
		i2c_send_option_motherboard(I2C_MOTHERBOARD_MUTE_OPTION, current.mute);
	}
}

void page_level_save() {}
	
static void show_level_bars(uint8_t x, uint8_t y)
{
	static uint8_t level_left = 39;
	static uint8_t level_right = 39;
	
	if (audio_level.left > level_left) {
		level_left++;
	}
	if (audio_level.left < level_left) {
		level_left--;
	}
	
	if (audio_level.right > level_right) {
		level_right++;
	}
	if (audio_level.right < level_right) {
		level_right--;
	}
		
 	oled_printf(x+2, y+36, FONTID_6X8M, "2");     oled_printf(x+6, y+36, FONTID_6X8M, "5");
 	oled_printf(x+2, y+21, FONTID_6X8M, "2");     oled_printf(x+6, y+21, FONTID_6X8M, "0");
	oled_printf(x+2, y+4, FONTID_6X8M,  "1");     oled_printf(x+6, y+4, FONTID_6X8M,  "5");

 	oled_printf(x+42, y+36, FONTID_6X8M, "2");     oled_printf(x+46, y+36, FONTID_6X8M, "5");
 	oled_printf(x+42, y+21, FONTID_6X8M, "2");     oled_printf(x+46, y+21, FONTID_6X8M, "0");
 	oled_printf(x+41, y+4, FONTID_6X8M,  "1");     oled_printf(x+45, y+4, FONTID_6X8M,  "5");	 
	 
	
	oled_draw_line(x+13, y+4,  x+13, y+45); // левая линия
	oled_draw_line(x+39, y+4,  x+39, y+45); // правая линия
	oled_draw_line(x+13, y+46, x+39, y+46); // нижняя линия
	
	oled_draw_pixel(x+40, y+42, 1);
	oled_draw_pixel(x+40, y+37, 1);
	oled_draw_pixel(x+40, y+28, 1);
	oled_draw_pixel(x+40, y+22, 1);
	oled_draw_pixel(x+40, y+11, 1);
	oled_draw_pixel(x+40, y+4,  1);

	oled_draw_pixel(x+12, y+42, 1);
	oled_draw_pixel(x+12, y+37, 1);
	oled_draw_pixel(x+12, y+28, 1);
	oled_draw_pixel(x+12, y+22, 1);
	oled_draw_pixel(x+12, y+11, 1);
	oled_draw_pixel(x+12, y+4,  1);


	show_level_bar(10, x+15, y+4, level_left, 40, 1);  // левый уровень
	show_level_bar(10, x+27, y+4, level_right, 40, 1); // правый уровень
}

static void show_freq()
{
	char str_buf[10];
	
	if (current_freq <= 20) {
		//oled_printf(5, 30, FONTID_6X8M, "частота: %d", current_freq * 50);
		oled_draw_menu_item(2, 49, FONTID_6X8M, current_select == SELECT_PAGE_LEVEL_FREQ ? 1:0, strcpy_P(str_buf, PSTR("част:%d")), current_freq * 50);

	} else {
		//oled_printf(5, 30, FONTID_6X8M, "частота: %d", current_freq * 100);
		oled_draw_menu_item(2, 49, FONTID_6X8M, current_select == SELECT_PAGE_LEVEL_FREQ ? 1:0, strcpy_P(str_buf, PSTR("част:%d")), current_freq * 100);
	}
}

static void show_menu(uint8_t x, uint8_t y)
{	
	char str_buf[15];
	
	oled_draw_menu_item(x, y, FONTID_6X8M, current_select == SELECT_PAGE_LEVEL_FIX_LEVEL ? 1:0, strcpy_P(str_buf, PSTR("фикс.ур:%d")), current.fix_level);
	y = y+10;
	oled_draw_menu_item(x, y, FONTID_6X8M, current_select == SELECT_PAGE_LEVEL_MUTE ? 1:0, strcpy_P(str_buf, PSTR("откл.зв:%d")), current.mute);
	y = y+10;
	
	if (is_rec_mode(current.servo_real_mode) == 1) {
		oled_draw_menu_item(x, y, FONTID_6X8M, current_select == SELECT_PAGE_LEVEL_BIAS ? 1:0, strcpy_P(str_buf, PSTR("подм:%02d")), current.bias);
		y = y+10;
		oled_draw_menu_item(x, y, FONTID_6X8M, current_select == SELECT_PAGE_LEVEL_UZ_EQ ? 1:0, strcpy_P(str_buf, PSTR("эkв:%d")), current.uz_eq);
	}
}