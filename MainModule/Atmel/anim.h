/*
 * anim.h
 *
 * ? Created: 30.07.2024 4:32:04
 *  Author: max4d
 */ 


#ifndef ANIM_H_
#define ANIM_H_

#include "main.h"

extern const uint8_t upm_stop_anim[292][3];
extern const uint8_t upm_stop_belts[149][3];
extern const uint8_t upm_forward_belts[162][3];
extern const uint8_t upm_rewind_belts[168][3];

void show_upm_anim();
void show_loading();

#endif /* ANIM_H_ */