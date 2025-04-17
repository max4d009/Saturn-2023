/*
 * repeat.c
 *
 * λ Created: 13.08.2024 3:04:31
 *  Author: max4d
 */ 

#include "repeat.h"

// Пока простенький алгорим повтора всей катушки
void repeat_program()
{
	if (kinematics_mode.repeat == 0) {
		return;
	}
	
	if (kinematics_mode.previous == PLAY_MODE) {
		set_mode(REWIND_MODE, 1, 0);
	}  else if (kinematics_mode.previous == REWIND_MODE) {
		set_mode(PLAY_MODE, 1, 0);
		kinematics_mode.repeat--;
	}
}