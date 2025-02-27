/*
 * manager_page.h
 *
 * Created: 17.08.2024 1:28:24
 *  Author: max4d
 */ 
#ifndef MANAGER_PAGE_H_
#define MANAGER_PAGE_H_

#include "main.h"

#define PAGE_OLED_TIMER 0
#define PAGE_OLED_SPEED 1
#define PAGE_OLED_SERVO_CONFIG 2
#define PAGE_OLED_IR_BINDING 3
#define PAGE_OLED_LEVEL 4
#define PAGE_OLED_FFT 5
#define PAGE_OLED_TIMER_REC 6
#define PAGE_OLED_OFF 7
#define PAGE_OLED_MENU 8
#define PAGE_OLED_VU 9
#define PAGE_OLED_TENSION_CONFIG 10

#define NUM_PAGES 11

#define REC_MENU_PAGES_NUM 3

struct Page {
	void (*render_page) (uint8_t) ;
	void (*menu_click) (void);
	void (*select_click) (void);
	void (*minus_click) (void);
	void (*plus_click) (void);
	void (*save_click) (void);
};
struct Page page_list[NUM_PAGES];

struct ConfigParam {
	uint8_t id;
	PGM_P name;
	uint8_t value;
};

void manager_page_initPages();
void manager_page_showPage();
void change_record_page();
void set_record_page(uint8_t rec_mode_inc);
void showConfigMenu(uint8_t num_params, struct ConfigParam *params, uint8_t current_param);

#endif /* MANAGER_PAGE_H_ */