/*
 * page_tension_config.h
 *
 * λ Created: 23.01.2025 2:39:54
 *  Author: max4d
 */ 
#ifndef PAGE_TENSION_CONFIG_H_
#define PAGE_TENSION_CONFIG_H_

#include "../main.h"

#define PID_REGULATOR_TENSION_PLAY 0
#define PID_REGULATOR_TENSION_FORWARD 1
#define PID_REGULATOR_TENSION_REWIND 2

#define CONFIG_TENSION_P 0
#define CONFIG_TENSION_I 1
#define CONFIG_TENSION_D 2

#define MENU_TENSION_CONFIG_COUNT_PARAMS 3
#define NUM_PID_REGULATOR 3

void init_page_tension_config();
void page_tension_config_render(uint8_t first_render);
void page_tension_config_menu();
void page_tension_config_select();
void page_tension_config_plus();
void page_tension_config_minus();
void page_tension_config_save();

#endif /* PAGE_TENSION_CONFIG_H_ */