/*
 * page_timer.c
 *
 * λ Created: 24.10.2023 18:20:14
 *  Author: m4d
 */ 
#include "page_timer.h"

static void show_current_mode();
static void show_left_menu();
static void show_timer();
static void show_mini_timer();


void page_timer_render(uint8_t first_render)
{

	if (first_render == 1) {
	}
	
	disp1color_FillScreenbuff(0);
    show_left_menu();
	show_current_mode();
	show_timer();
	show_mini_timer();	
	timer_search_timer();	
	disp1color_UpdateFromBuff();
}

void page_timer_menu() {
	current.page = PAGE_OLED_MENU;
}

void page_timer_select()
{
	timer_search_reset();
	timer_reset();
}

void page_timer_minus() 
{
	timer_search_dec_target_time();
}
	
void page_timer_plus() 
{
	timer_search_inc_target_time();
}

void page_timer_save() 
{
	timer_search_start_search();
}	

static void show_timer() 
{	
	oled_printf(43, 18, FONTID_10X16F, "%d.%d%d.%d%d.%d", timer.hour, timer.min/10, timer.min%10, timer.sec/10, timer.sec%10, timer.m_sec);
	oled_show_conter_background();
}

static void show_left_menu()
{
	if (current.nr == 1) {
		oled_printf(2, 25, FONTID_6X8M, "сшп");
	}

	static uint8_t eq_timer = 30;

	if (current.eq == 0 && eq_timer < 30) {
		oled_show_eq(current.eq);
		eq_timer++;
	} else if (current.eq > 0) {
		eq_timer = 0;
		oled_show_eq(current.eq);
	}

	if (current.kontr == 1) {
		oled_printf(2, 7, FONTID_6X8M, "вы"); oled_printf(14, 7, FONTID_6X8M, "ход");
	} else {
		oled_printf(2, 7, FONTID_6X8M, "в"); oled_printf(8, 7, FONTID_6X8M, "ход");
	}
}

static void show_current_mode()
{
	static uint8_t in_process_inc = 0;
	
	if (current.in_process == 1) {
		if (in_process_inc <= 6) {
			in_process_inc++;
			return;
		}
		
		in_process_inc++;
		if (in_process_inc > 10) {
			in_process_inc = 0;
		}
	} else {
		in_process_inc = 0;
	}
	
	switch (current.servo_real_mode) {
		case STOP_MODE:
			oled_show_stop(44, 9);
		break;
		case PLAY_MODE:
			oled_show_play(44, 9);
		break;
		case REWIND_MODE:
		case REWIND_LITTLE_MODE:		
			oled_show_rewind(44, 9);
		break;
		case REC_MODE:
		case REC_MODE_PLAY:
			oled_printf(42, 7, FONTID_6X8M, "ЗАПИСЬ!");
		break;
		case FORWARD_MODE:
		case FORWARD_LITTLE_MODE:		
			oled_show_forward(44, 9);
		break;
		case REWIND_SEARCH_MODE:
			oled_show_rewind(44, 9);
			oled_printf(64, 7, FONTID_6X8M, "поиск");
		break;
		case FORWARD_SEARCH_MODE:
			oled_show_forward(44, 9);
			oled_printf(64, 7, FONTID_6X8M, "поиск");
		break;
		case PAUSE_MODE:
			oled_show_pause(44, 9);
		break;
	}
	
	if (current.repeat != 0) {
		oled_show_repeat(111, 5, current.repeat);
	}
}

static void show_mini_timer()
{
	int16_t search_target_time = timer_search_get_target_time();
	if (search_target_time == 0) {
		oled_printf(24, 56, FONTID_6X8M, "0");
		oled_printf(28, 56, FONTID_6X8M, "0");
		oled_printf(32, 56, FONTID_6X8M, ":");
		oled_printf(34, 56, FONTID_6X8M, "0");
		oled_printf(39, 56, FONTID_6X8M, "0");
		return;
	}
	
	uint8_t time_abs_min = abs(search_target_time) / 60;
	uint8_t time_abs_sec = abs(search_target_time) % 60;
	
	if (search_target_time < 0) {
		oled_printf(23, 57, FONTID_6X8M, "-");
	}
	oled_printf(24, 56, FONTID_6X8M, "%d", time_abs_min > 9 ? time_abs_min / 10 : 0);
	oled_printf(28, 56, FONTID_6X8M, "%d", time_abs_min > 9 ? time_abs_min % 10 : time_abs_min);
	oled_printf(32, 56, FONTID_6X8M, ":");
	oled_printf(34, 56, FONTID_6X8M, "%d", time_abs_sec > 9 ? time_abs_sec / 10 : 0);
	oled_printf(39, 56, FONTID_6X8M, "%d", time_abs_sec > 9 ? time_abs_sec % 10 : time_abs_sec);
}