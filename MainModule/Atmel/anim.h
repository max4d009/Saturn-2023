/*
 * anim.h
 *
 * λ Created: 30.07.2024 4:32:04
 *  Author: max4d
 */ 


#ifndef ANIM_H_
#define ANIM_H_

#include "main.h"

extern const uint8_t upm_stop_anim[292][4];
extern const uint8_t upm_stop_belts[149][4];
extern const uint8_t upm_forward_belts[162][4];
extern const uint8_t upm_rewind_belts[168][4];

void show_upm_anim();
void show_loading();

#endif /* ANIM_H_ */