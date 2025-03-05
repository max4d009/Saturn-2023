/*
 * page_menu.h
 *
 * λ Created: 17.08.2024 4:04:46
 *  Author: max4d
 */ 
#ifndef PAGE_MENU_H_
#define PAGE_MENU_H_

#include "../main.h"

#define PAGE_MENU_SELECT_PAGE_SERVO_CONFIG 0
#define PAGE_MENU_SELECT_PAGE_LEVEL 1
#define PAGE_MENU_SELECT_PAGE_SPEED 2
#define PAGE_MENU_SELECT_PAGE_IR 3
#define PAGE_MENU_SELECT_PAGE_VU 4
#define PAGE_MENU_SELECT_PAGE_TENSION_CONFIG 5

#define PAGE_MENU_ITEMS_COUNT 6	

#define PAGE_MENU_ITEMS_ONE_PAGE_COUNT 6


struct PageMenu {
	uint8_t page_id;
	PGM_P name;
};

struct PageMenu page_menu[PAGE_MENU_ITEMS_COUNT];		

void page_menu_render(uint8_t first_render);
void page_menu_menu();
void page_menu_select();
void page_menu_minus();
void page_menu_plus();
void page_menu_save();						

#endif /* PAGE_MENU_H_ */