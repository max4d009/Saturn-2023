/*
 * main.h
 *
 * λ Author: max4d
 */ 
#ifndef _MAIN_H
#define _MAIN_H

//#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <compat/twi.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "Display/disp1color.h"
#include "Display/font.h"
#include "i2c.h"
#include "adc.h"
#include "buttons.h"
#include "buttons_adc.h"
#include "Pages/page_timer.h"
#include "Pages/page_fft.h"
#include "Pages/page_servo_config.h"
#include "ir_contr.h"
#include "eeprom.h"
#include "Pages/page_ir.h"
#include "Pages/page_level.h"
#include "anim.h"
#include "Pages/page_speed.h"
#include "Pages/page_off.h"
#include "counter.h"
#include "manager_page.h"
#include "debug_functions.h"
#include "Pages/page_menu.h"
#include "Pages/page_vu.h"
#include "spim.h"
#include "Pages/page_tension_config.h"
#include "time_search.h"

#define REC_MODE 0
#define PLAY_MODE 1
#define STOP_MODE 2
#define REWIND_MODE 3
#define FORWARD_MODE 4
#define PAUSE_MODE 5
#define REWIND_SEARCH_MODE 6
#define FORWARD_SEARCH_MODE 7
#define REC_MODE_GEN 8
#define REC_MODE_PLAY 9
#define REWIND_LITTLE_MODE 10
#define FORWARD_LITTLE_MODE 11


#define NR_OPTION 0
#define EQ_OPTION 1
#define KONTR_OPTION 2
#define GEN_OPTION 3
#define PAGE_OLED_OPTION 4

#define IR_LED_PIN PD4
#define STAND_BY_PIN PD5
#define IR_PIN PD2
#define REC_LED_PIN PD7
#define CHANNEL_LEFT 0
#define CHANNEL_RIGHT 1
#define DEBUG_STR_ALL_LEN 15

#define FPSTR(pstr) (const __FlashStringHelper*)(pstr)

struct CurrentParameters {
	uint8_t on;
	uint8_t prev_mode;
	uint8_t servo_real_mode;
	uint8_t nr;
	uint8_t eq;
	uint8_t kontr;
	uint8_t uz_eq;
	uint8_t fix_level;
	uint8_t bias;
	uint8_t repeat;
	uint8_t page;
	uint8_t debug;
	uint8_t reel_speed_left;
	uint8_t reel_speed_right;
	uint8_t motor_speed;
	uint8_t kinematics_speed;
	uint8_t in_process;
	uint8_t tension;
	uint8_t mute;
	uint8_t vu_peaks_mode;
	uint8_t vu_peaks_time;
	uint8_t reel_size;
};
struct CurrentParameters current;

struct AudioLevel {
	uint8_t left;
	uint8_t right;
	uint8_t updated;
};

struct AudioLevel audio_level;

const uint8_t REC_MENU_PAGES[3];

#define DEBUG_STR_LEFT_MAX_LEN 8

char debug_string[DEBUG_STR_LEFT_MAX_LEN];

void send_debug_int_var_oled(char *varName, uint16_t varValue);
void resetParameters();
void send_debug_var_oled_left(char *varValue);
uint8_t is_rec_mode(uint8_t mode);
uint8_t is_rec_gen_mode();

#endif