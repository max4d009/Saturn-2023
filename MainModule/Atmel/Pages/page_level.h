/*
 * pageLevel.h
 *
 * λ Created: 19.07.2024 22:04:43
 *  Author: max4d
 */ 

#ifndef PAGELEVEL_H_
#define PAGELEVEL_H_

#include "../main.h"

#define SELECT_PAGE_LEVEL_FIX_LEVEL 0
#define SELECT_PAGE_LEVEL_MUTE 1
#define SELECT_PAGE_LEVEL_BIAS 2
#define SELECT_PAGE_LEVEL_UZ_EQ 3
#define SELECT_PAGE_LEVEL_FREQ 4

#define SELECT_PAGE_LEVEL_NUM_REC 5
#define SELECT_PAGE_LEVEL_NUM 4

void page_level_render(uint8_t first_render);
void page_level_menu();
void page_level_select();
void page_level_minus();
void page_level_plus();
void page_level_save();

#endif /* PAGELEVEL_H_ */