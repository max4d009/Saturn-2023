/*
 * counter.c
 *
 * Created: 17.08.2024 1:19:07
 *  Author: max4d
 */ 
#include "counter.h"

void timer_reset()
{
	timer.m_sec = 0;
	timer.sec = 0;
	timer.min = 0;
	timer.hour = 0;
	timer.buf_up = 0;
	timer.buf_down = 0;
	timer.all_sec = 0;
}

void counterIn(uint16_t count)
{
	if (count == 0) {
		return;
	}
	
	if (timer.buf_up < 65000 && count < 500) {
		timer.buf_up = timer.buf_up + count;
	}
}

void counterOut(uint16_t count)
{
	if (count == 0) {
		return;
	}
	if (timer.buf_down < 65000 && count < 500) {
		timer.buf_down = timer.buf_down + count;
	}
}

void inc_counter()
{
	uint8_t d_sec = 6;
	uint8_t chunk_up = 0;
	uint8_t chunk_down = 0;
	uint16_t buffer_up = 0;
	uint16_t buffer_donw = 0;

	buffer_up = timer.buf_up;
	while (buffer_up >= d_sec && buffer_up < 65000) {
		chunk_up = chunk_up + d_sec;
		buffer_up = buffer_up - d_sec;
	}
	timer.buf_up = timer.buf_up - chunk_up;
	chunk_up = (chunk_up) / d_sec;
	
	if (chunk_up > 0) {
		for (uint8_t i = 0; i < chunk_up; i++) {
			timer.m_sec++;
			if (timer.m_sec == 10) {
				timer.sec++;
				if (timer.all_sec < 30000) {
					timer.all_sec++;
				}
				timer.m_sec = 0;
				if (timer.sec == 60) {
					timer.min++;
					timer.m_sec = 0; timer.sec = 0;
					if (timer.min == 60) {
						timer.hour++;
						timer.m_sec = 0; timer.sec = 0; timer.min = 0;
						if (timer.hour >= 5) {
							timer.hour = 0; timer.m_sec = 0; timer.sec = 0; timer.min = 0; timer.hour = 0;
						}
					}
				}
			}
		}
	}
	
	buffer_donw = timer.buf_down;
	while (buffer_donw >= d_sec && buffer_up < 65000) {
		chunk_down = chunk_down + d_sec;
		buffer_donw = buffer_donw - d_sec;
	}
	timer.buf_down = timer.buf_down - chunk_down;
	chunk_down = chunk_down / d_sec;
	
	if (chunk_down > 0) {
		for (uint8_t i = 0; i < chunk_down; i++) {
			timer.m_sec--;
			if (timer.m_sec == -1) {
				timer.sec--;
				if (timer.all_sec > -30000) {
					timer.all_sec--;
				}
				timer.m_sec = 9;
				if (timer.sec == -1) {
					timer.min--; // Увеличиваем минуты
					timer.m_sec = 9; timer.sec = 59;
					if (timer.min == -1) {
						timer.hour--; // Увеличиваем часы
						timer.m_sec = 9; timer.sec = 59; timer.min = 59;
						if (timer.hour == -1) { // Если прошел час обнуляем все
							timer.m_sec = 9; timer.sec = 59; timer.min = 59; timer.hour = 5;
						}
					}
				}
			}
		}
	}
}