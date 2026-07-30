/*
 * page_eq.h
 *
 * Created: 18.06.2026 0:56:00
 *  Author: max4d
 */ 


#ifndef PAGE_EQ_H_
#define PAGE_EQ_H_

#include "../main.h"

#define GAUGE_WIDGETS_COUNT 5
#define TEXT_WIDGETS_COUNT 1
#define ALL_WIDGETS_COUNT 6

#define TEXT_WIDGET_EQ     0

#define GAUGE_WIDGET_GEQ_LOW  0
#define GAUGE_WIDGET_GEQ_HIGHT  1
#define GAUGE_WIDGET_BBE  2
#define GAUGE_WIDGET_TBASS  3
#define GAUGE_WIDGET_VOL  4

// ID элементов EQ
#define TEXT_EQ_ROCK    0
#define TEXT_EQ_POP     1
#define TEXT_EQ_CLASSIC 2


void page_eq_render(uint8_t first_render);
void page_eq_menu();
void page_eq_select();
void page_eq_minus();
void page_eq_plus();
void page_eq_save();


#endif /* PAGE_EQ_H_ */