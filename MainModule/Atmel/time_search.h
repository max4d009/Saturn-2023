/*
 * time_search.h
 *
 * Created: 24.01.2025 2:33:30
 *  Author: max4d
 */ 
#ifndef TIME_SEARCH_H_
#define TIME_SEARCH_H_

#include "main.h"

#define SEARCH_START_SPEED 90
#define SEARCH_LOW_SPEED 60
#define SEARCH_END_SPEED 20
#define SEARCH_SECONDS_BEFORE_SET_MIDDLE_SPEED 50

#define STEP_OFF -1
#define STEP_0_SET_SEARCH_SPEED 0
#define STEP_1_SET_MIDDLE_SPEED 1
#define STEP_2_WAIT_MIDDLE_SPEED_STABILIZE 2
#define STEP_3_SET_LOW_SPEED 3
#define STEP_4_END 4


void timer_search_inc_target_time();
void timer_search_dec_target_time();
void timer_search_start_search();
void timer_search_reset();
void timer_search_timer();
int16_t timer_search_get_target_time();

#endif /* TIME_SEARCH_H_ */