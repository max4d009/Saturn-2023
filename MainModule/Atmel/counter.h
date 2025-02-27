/*
 * counter.h
 *
 * Created: 17.08.2024 1:19:20
 *  Author: max4d
 */ 
#ifndef COUNTER_H_
#define COUNTER_H_

#include "main.h"

struct Timer {
	int8_t m_sec;
	int8_t sec;
	int8_t min;
	int8_t hour;
	uint16_t buf_up;
	uint16_t buf_down;
	int16_t all_sec;
};
struct Timer timer;

void inc_counter();
void counterIn(uint16_t count);
void counterOut(uint16_t count);
void timer_reset();

#endif /* COUNTER_H_ */