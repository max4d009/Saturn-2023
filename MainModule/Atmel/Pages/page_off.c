/*
 * page_off.c
 *
 * Created: 16.08.2024 2:37:20
 *  Author: max4d
 */ 
#include "page_off.h"

static void showWhiteNoise(uint8_t height_bias);

void page_off_render(uint8_t first_render)
{
	char str_buf[15];
	
	static uint8_t height_bias = 0;
	static uint8_t line_bias = 0;
	if (first_render == 1) {
		height_bias = 0;
		line_bias = 0;
	}
	
	if (height_bias <= 30) {
		height_bias++;height_bias++;height_bias++;
		showWhiteNoise(height_bias);
	}
	
	if (height_bias < 12) {
		oled_printf(5, 10,  FONTID_6X8M, strcpy_P(str_buf, PSTR("Выключение...")));
	}
	
	if (height_bias < 18) {
		oled_printf(43, 18, FONTID_10X16F, "%d.%d%d.%d%d.%d",0,0,0,0,0,0);
		oled_show_conter_background();
	}
	
	if (height_bias == 31) {
		oled_draw_line(0, 32, 128, 32);
		height_bias++;
	}
	
	if (height_bias > 31 && line_bias < 63) {
		oled_draw_line(line_bias, 30, 128-line_bias, 30);
		oled_draw_line(line_bias, 30, 128-line_bias, 30);
		line_bias = line_bias + 6;
	}
	
	if (line_bias > 63) {
		oled_draw_line(63, 30, 64, 30);
		oled_draw_line(63, 31, 64, 31);
	}
}

static void showWhiteNoise(uint8_t height_bias)
{
	for (uint8_t i = 0; i <= 128; i++) {
		i++;
		for (uint8_t g = height_bias; g <= 64 - height_bias; g++) {
			g++;
			oled_draw_pixel(i, g, rand()%2);
		}
	}
	disp1color_UpdateFromBuff();
}

void page_off_menu() {}
void page_off_select() {}
void page_off_minus() {}
void page_off_plus() {}
void page_off_save() {}