/*
 * audio.c
 *
 * λ Created: 12.08.2022 2:53:37
 *  Author: m4d
 */ 

#include "audio.h"

uint16_t dbs[40] = {
	88,86,84,82,80,
	78,76,74,72,70,
	68,66,64,62,60,
	58,56,54,52,50,
	48,46,44,42,40,
	39,38,37,36,35,
	34,33,32,31,30,
	29,28,27,26,25
};

static char getDbsByAdc(uint16_t adc);

void audio_init()
{
	audio_level.left = 0;
	audio_level.right = 0;
}

void audio_timer_left(uint16_t adc)
{
	audio_level.left = getDbsByAdc(adc);
}

void audio_timer_right(uint16_t adc)
{
	audio_level.right = getDbsByAdc(adc);
}

static char getDbsByAdc(uint16_t adc)
{
	for (uint8_t i = 0; i <= 39; i++){
		if (adc > dbs[i]) {
			return i;
		}
	}
	return 40;
}