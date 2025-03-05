/*
 * page_vu.c
 *
 * λ Created: 19.08.2024 3:02:24
 *  Author: max4d
 */ 
#include "page_vu.h"

static int8_t current_select = 0;

static void show_vu_menu(uint8_t x, uint8_t y);

void page_vu_render(uint8_t first_render)
{
	if (first_render == 1) {
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_MODE].id_item = VU_CONFIG_MENU_ITEM_PEAKS_MODE;
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_MODE].name = PSTR("Отобр. пики");
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_MODE].min = 0;
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_MODE].max = 3;
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_MODE].current = current.vu_peaks_mode;
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_MODE].i2c_symbol = I2C_VU_START_TRANSACTION_PEAKS_MODE;
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_MODE].i2c_symbol_save = I2C_VU_START_TRANSACTION_PEAKS_MODE_SAVE;
		
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_TIME].id_item = VU_CONFIG_MENU_ITEM_PEAKS_TIME;
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_TIME].name = PSTR("Время отобр.");
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_TIME].min = 0;
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_TIME].max = 25;
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_TIME].current = current.vu_peaks_time;
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_TIME].i2c_symbol = I2C_VU_START_TRANSACTION_PEAKS_TIME;
		vu_menu[VU_CONFIG_MENU_ITEM_PEAKS_TIME].i2c_symbol_save = I2C_VU_START_TRANSACTION_PEAKS_TIME_SAVE;
	}
	
	show_vu_menu(2, 0);
}

void page_vu_menu()
{
}

void page_vu_select()
{
	current_select++;
	if (current_select > VU_CONFIG_MENU_COUNT - 1) {
		current_select = 0;
	}
}

void page_vu_minus() 
{
	vu_menu[current_select].current--;
	if (vu_menu[current_select].current < vu_menu[current_select].min) {
		vu_menu[current_select].current = vu_menu[current_select].min;
	}
	current.vu_peaks_mode = vu_menu[current_select].current;
	i2c_send_vu_params(vu_menu[current_select].i2c_symbol, vu_menu[current_select].current);
}

void page_vu_plus() 
{
	vu_menu[current_select].current++;
	if (vu_menu[current_select].current > vu_menu[current_select].max) {
		vu_menu[current_select].current = vu_menu[current_select].max;
	}
	current.vu_peaks_mode = vu_menu[current_select].current;
	i2c_send_vu_params(vu_menu[current_select].i2c_symbol, vu_menu[current_select].current);
}

void page_vu_save() 
{
	i2c_send_vu_params(vu_menu[current_select].i2c_symbol_save, vu_menu[current_select].current);
}
	
static void show_vu_menu(uint8_t x, uint8_t y)
{
	uint8_t yy = y;
	
	for (uint8_t i = 0; i < VU_CONFIG_MENU_COUNT; i++) {
		
		char value_char[4];
		memset(value_char, '\0', 4);
		itoa(vu_menu[i].current, value_char, 10);
		
		char menu_item_name[20];
		strcpy_P(menu_item_name, vu_menu[i].name);
		
		strcat(menu_item_name, ":");
		strcat(menu_item_name, value_char);
		
		oled_draw_menu_item(x, yy, FONTID_6X8M, current_select == i ? 1:0, menu_item_name);
		yy = yy + 10;
	}
}