/*
 * page_vu.h
 *
 * λ Created: 19.08.2024 3:02:36
 *  Author: max4d
 */ 
#ifndef PAGE_VU_H_
#define PAGE_VU_H_

#include "../main.h"

#define VU_CONFIG_MENU_ITEM_PEAKS_MODE 0
#define VU_CONFIG_MENU_ITEM_PEAKS_TIME 1

#define VU_CONFIG_MENU_COUNT 2

struct VuMenu {
	uint8_t id_item;
	uint8_t min;
	uint8_t max;
	int8_t current;
	PGM_P name;
	char i2c_symbol;
	char i2c_symbol_save;
};

struct VuMenu vu_menu[VU_CONFIG_MENU_COUNT];

void page_vu_render(uint8_t first_render);
void page_vu_menu();
void page_vu_select();
void page_vu_minus();
void page_vu_plus();
void page_vu_save();

#endif /* PAGE_VU_H_ */