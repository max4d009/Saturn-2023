/*
 * audio.h
 *
 * Created: 12.08.2022 2:53:53
 *  Author: m4d
 */ 
#ifndef AUDIO_H_
#define AUDIO_H_

#include "main.h"

void audio_timer_right(uint16_t adc);
void audio_timer_left(uint16_t adc);
void audio_init();
void enable_audio_level_check(uint8_t enable);

struct AudioLevel {
	uint8_t left;
	uint8_t right;
};

struct AudioLevel audio_level;

#endif /* AUDIO_H_ */