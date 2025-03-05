/*
 * pageIR.c
 *
 * λ Created: 09.12.2023 3:57:39
 *  Author: max-4d
 */ 
#include "page_ir.h"

void page_ir_render(uint8_t first_render)
{
	char str_buf[12];
	
	oled_printf(0, 0,  FONTID_6X8M, strcpy_P(str_buf, PSTR("Привязка ДУ")));	
	oled_printf(0, 10, FONTID_6X8M, strcpy_P(str_buf, PSTR("Нажм. выбор")));
	oled_printf(0, 50, FONTID_6X8M, debug_string);
}

void page_ir_menu() 
{
	//current.page = PAGE_OLED_TIMER;
}

void page_ir_select()
{
	ir_binding();
}

void page_ir_minus() {}
void page_ir_plus() {}
void page_ir_save() {}