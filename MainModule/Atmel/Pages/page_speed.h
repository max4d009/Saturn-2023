/*
 * page_speed.h
 *
 * λ Created: 31.07.2024 3:22:38
 *  Author: max4d
 */ 
#ifndef PAGE_SPEED_H_
#define PAGE_SPEED_H_

#include "../main.h"

#define PAGE_SPEED_SELECT_REEL_SIZE 0
#define PAGE_SPEED_SELECT_MOTOR_SPEED 1
#define PAGE_SPEED_SELECT_KINEMATICS_SPEED 2
#define PAGE_SPEED_SELECT_NUM 3

void page_speed_render(uint8_t first_render);
void page_speed_menu();
void page_speed_select();
void page_speed_minus();
void page_speed_plus();
void page_speed_save();

#endif /* PAGE_SPEED_H_ */