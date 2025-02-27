/*
 * CFile1.c
 *
 * Created: 17.08.2024 4:04:15
 *  Author: max4d
 */ 
#include "page_menu.h"

static uint8_t current_select = 0;
static uint8_t current_page = 0;
static void show_main_menu(uint8_t x, uint8_t y, uint8_t page);

void page_menu_render(uint8_t first_render)
{
	if (first_render == 1) {
		page_menu[0].page_id = PAGE_OLED_SERVO_CONFIG;
		page_menu[0].name = PSTR("Настр. серв");
		
		page_menu[1].page_id = PAGE_OLED_LEVEL;
		page_menu[1].name = PSTR("Уровень");
		
		page_menu[2].page_id = PAGE_OLED_SPEED;
		page_menu[2].name = PSTR("Скорость");
		
		page_menu[3].page_id = PAGE_OLED_IR_BINDING;
		page_menu[3].name = PSTR("Привязка ДУ");
		
		page_menu[4].page_id = PAGE_OLED_VU;
		page_menu[4].name = PSTR("Индикаторы");
		
		page_menu[5].page_id = PAGE_OLED_TENSION_CONFIG;
		page_menu[5].name = PSTR("Натяжение");
	}
		
	show_main_menu(2, 0, current_page);
	
	oled_printf(120, 0, FONTID_6X8M, "%d", current_page);
}

static void show_main_menu(uint8_t x, uint8_t y, uint8_t page)
{
	uint8_t yy = y;
	
	uint8_t on_page_menu_index = 0;
	for (uint8_t i = current_page * PAGE_MENU_ITEMS_ONE_PAGE_COUNT; i < PAGE_MENU_ITEMS_COUNT; i++) {
		if (on_page_menu_index == PAGE_MENU_ITEMS_ONE_PAGE_COUNT) {
			break;
		}
		on_page_menu_index++;
		
		char menu_item_name[strlen_P(page_menu[i].name) + 1];
		strcpy_P(menu_item_name, page_menu[i].name);
		
		oled_draw_menu_item(x, yy, FONTID_6X8M, current_select == i ? 1:0, menu_item_name);
		yy = yy + 10;
	}	
}

void page_menu_menu() 
{
	current_select++;
	if (current_select > PAGE_MENU_ITEMS_COUNT-1) {
		current_select = 0;
		current_page = 0;
		return;
	}
	
	
	if (current_select > PAGE_MENU_ITEMS_ONE_PAGE_COUNT-1) {
		current_page++;
		return;
	}
}
void page_menu_select() 
{
	current.page = page_menu[current_select].page_id;
}
void page_menu_minus() {}
void page_menu_plus() {}
void page_menu_save() {}