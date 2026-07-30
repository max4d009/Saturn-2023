/*
 * page_off.c
 *
 * ? Created: 16.08.2024 2:37:20
 *  Author: max4d
 */ 
#include "page_off.h"

void page_off_render(uint8_t first_render)
{	
	disp1color_FillScreenbuff(0);
	show_upm_anim();
	disp1color_UpdateFromBuff();
}

void page_off_menu() {}
void page_off_select() {}
void page_off_minus() {}
void page_off_plus() {}
void page_off_save() {}