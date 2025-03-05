/*
 * servo_mode_control.c
 *
 * λ Created: 09.07.2024 19:04:28
 *  Author: max4d
 */ 

#include "servo_mode_control.h"

static uint8_t play_timer();
static uint8_t pause_timer();
static uint8_t forward_timer();
static uint8_t rewind_timer();
static uint8_t stop_timer(uint8_t long_wait);
static uint8_t search_on_off_check(uint8_t mode);
static uint8_t little_forward_timer();
static uint8_t little_rewind_timer();
static uint8_t is_pause_play_switch();
static uint8_t need_stop_mode();
static uint8_t little_forward_timer_off();
static uint8_t little_rweind_timer_off();


static uint8_t search_on_off_check(uint8_t mode)
{
	if (kinematics_mode.in_search == 0 && (mode == FORWARD_SEARCH_MODE || mode == REWIND_SEARCH_MODE)) {
		kinematics_mode.current = mode;
		set_motor_speed(SEARCH_SPEED, 0);
		servo_list[SERVO_PLAY].need_angle = servo_list[SERVO_PLAY].search_angle;
		kinematics_mode.in_search = 1;
		return 1;
	}

	if (kinematics_mode.in_search == 1) {
		if (mode == FORWARD_MODE && kinematics_mode.current == FORWARD_SEARCH_MODE) {
			servo_list[SERVO_PLAY].need_angle = servo_list[SERVO_PLAY].forward_angle;
			kinematics_mode.current = mode;
			set_motor_speed(SPEED_FULL, 0);
			kinematics_mode.in_search = 0;
			return 1;
		}
	
		if (mode == REWIND_MODE && kinematics_mode.current == REWIND_SEARCH_MODE) {
			servo_list[SERVO_PLAY].need_angle = servo_list[SERVO_PLAY].rewind_angle;
			kinematics_mode.current = mode;
			set_motor_speed(SPEED_FULL, 0);
			kinematics_mode.in_search = 0;
			return 1;
		}		
	}
	
	return 0;
}

void set_mode(uint8_t mode, uint8_t force_off_search, uint8_t is_autostop)
{
	if (kinematics_mode.in_process == 1) {
		return;
	}
	
	if (force_off_search == 1) {
		kinematics_mode.in_search = 0;
	}
	
	if (search_on_off_check(mode) == 1) {
		return;
	}
	
	if (kinematics_mode.current != mode) {
		kinematics_mode.change_mode_counter = 0;
		kinematics_mode.previous = kinematics_mode.current;
		kinematics_mode.autostop_activated = is_autostop;
		kinematics_mode.current = mode;
		kinematics_mode.in_process = 1;
	}
}

static uint8_t need_stop_mode()
{
	if (is_pause_play_switch() == 1) {
		return 0;
	}
	
	if (kinematics_mode.previous == PLAY_MODE && (kinematics_mode.current == FORWARD_LITTLE_MODE || kinematics_mode.current == REWIND_LITTLE_MODE)) {
		return 0;
	}
	
	if ((kinematics_mode.previous == FORWARD_LITTLE_MODE || kinematics_mode.previous == REWIND_LITTLE_MODE) && kinematics_mode.current == PLAY_MODE) {
		return 0;
	}
	
	if (kinematics_mode.previous != STOP_MODE && kinematics_mode.current != STOP_MODE) {
		return 1;
	}
	
	return 0;
}

static uint8_t is_pause_play_switch()
{
	if ((kinematics_mode.current == PLAY_MODE || kinematics_mode.current == REC_MODE_PLAY) && kinematics_mode.previous == PAUSE_MODE) {
		return 1;
	} else if (kinematics_mode.current == PAUSE_MODE && (kinematics_mode.previous == PLAY_MODE || kinematics_mode.previous == REC_MODE_PLAY)) {
		return 1;
	}
	
	return 0;
}


// X4 Вызывается таймером постоянно. Начинает выполнение, когда произошло изменение режима.
void change_mode_timer()
{	
	
	if (kinematics_mode.in_process == 0) {
		return;
	}
	
	if (need_stop_mode() == 1) {
		if (stop_timer(1) == 0 && kinematics_mode.change_mode_counter < 500) {
			kinematics_mode.change_mode_counter++;
			return;
		}
		
		kinematics_mode.previous = STOP_MODE;
		kinematics_mode.slow_rew = 0;
		kinematics_mode.change_mode_counter = 0;
	}
	
	uint8_t result;
	switch (kinematics_mode.current) {
		case STOP_MODE:
			result = stop_timer(0);
			break;
		case PLAY_MODE:
		case REC_MODE_PLAY:
			result = play_timer();
			break;
		case PAUSE_MODE:
			result = pause_timer();
			break;
		case FORWARD_MODE:
			result = forward_timer();
			break;
		case REWIND_MODE:
			result = rewind_timer();
			break;
		case REC_MODE:
		case REC_MODE_GEN:
			result = 1;
			break;
		case FORWARD_LITTLE_MODE:
			result = little_forward_timer();
			break;	
		case REWIND_LITTLE_MODE:
			result = little_rewind_timer();
			break;				
		default:
			result = stop_timer(0);
		break;
	}
	
	if (result == 0 && kinematics_mode.change_mode_counter < 500) {
		kinematics_mode.change_mode_counter++;
	} else if (result == 1) {
		kinematics_mode.slow_rew = 0;
		kinematics_mode.change_mode_counter = 0;
		kinematics_mode.in_process = 0;
		
		if (kinematics_mode.current == STOP_MODE && kinematics_mode.autostop_activated) {
			kinematics_mode.autostop_activated = 0;
			repeat_program();
		}
	}
}

static uint8_t play_timer()
{
	if (kinematics_mode.previous == FORWARD_LITTLE_MODE) {
		return little_forward_timer_off();
	} else if (kinematics_mode.previous == REWIND_LITTLE_MODE) {
		return little_rweind_timer_off();
	}
	
	if (kinematics_mode.change_mode_counter == 0) {
		set_motor_speed(1000 + kinematics_mode.motor_speed_play_correction, 1);
		if (kinematics_mode.kinematics_speed == 0) {
			servo_list[SERVO_RIGHT].speed  = 2;
		} else {
			servo_list[SERVO_RIGHT].speed  = 1;
		}
		
		servo_list[SERVO_LEFT].speed   = 1;
		servo_list[SERVO_PLAY].speed   = 1;
		servo_list[SERVO_REWIND].speed = 1;
		servo_list[SERVO_RIGHT].need_angle  = servo_list[SERVO_RIGHT].play_angle;
		
	} else if (kinematics_mode.change_mode_counter == 30) {
		servo_list[SERVO_REWIND].need_angle = servo_list[SERVO_REWIND].play_angle;
		servo_list[SERVO_LEFT].need_angle   = servo_list[SERVO_LEFT].play_angle;
	} else if (kinematics_mode.change_mode_counter == 40) {
		servo_list[SERVO_PLAY].need_angle   = servo_list[SERVO_PLAY].play_angle;
		pidReset(servo_list[SERVO_LEFT].current_angle);
		kinematics_mode.tension_sensor_enable = 1;
		return 1;
	}
	return 0;
}

static uint8_t pause_timer()
{
	if (kinematics_mode.change_mode_counter == 0) {
		set_motor_speed(1000 + kinematics_mode.motor_speed_play_correction, 1);
		if (kinematics_mode.kinematics_speed == 0) {
			servo_list[SERVO_RIGHT].speed  = 2;
		} else {
			servo_list[SERVO_RIGHT].speed  = 1;
		}
		servo_list[SERVO_LEFT].speed   = 1;
		servo_list[SERVO_PLAY].speed   = 1;
		servo_list[SERVO_REWIND].speed = 1;
		servo_list[SERVO_RIGHT].need_angle  = servo_list[SERVO_RIGHT].pause_angle;
	} else if (kinematics_mode.change_mode_counter == 30) {
		servo_list[SERVO_REWIND].need_angle = servo_list[SERVO_REWIND].pause_angle;
		servo_list[SERVO_LEFT].need_angle   = servo_list[SERVO_LEFT].pause_angle;
	} else if (kinematics_mode.change_mode_counter == 40) {
		servo_list[SERVO_PLAY].need_angle   = servo_list[SERVO_PLAY].pause_angle;
		pidReset(servo_list[SERVO_LEFT].current_angle);
		kinematics_mode.tension_sensor_enable = 1;
		return 1;
	}
	return 0;
}

static uint8_t forward_timer()
{
	if (kinematics_mode.change_mode_counter == 0) {
		if (kinematics_mode.kinematics_speed == 0) {
			servo_list[SERVO_RIGHT].speed  = 2;
			servo_list[SERVO_LEFT].speed   = 2;
			servo_list[SERVO_REWIND].speed = 2;
		} else {
			servo_list[SERVO_RIGHT].speed  = 1;
			servo_list[SERVO_LEFT].speed   = 1;
			servo_list[SERVO_REWIND].speed = 1;
		}
		servo_list[SERVO_PLAY].speed = 1;
		servo_list[SERVO_RIGHT].need_angle  = servo_list[SERVO_RIGHT].forward_angle;
		servo_list[SERVO_LEFT].need_angle   = servo_list[SERVO_LEFT].forward_angle;
		servo_list[SERVO_PLAY].need_angle   = servo_list[SERVO_PLAY].forward_angle;

	} else if (kinematics_mode.change_mode_counter == 10) {
		servo_list[SERVO_REWIND].need_angle = servo_list[SERVO_REWIND].forward_angle;
		servo_list[SERVO_RIGHT].need_angle  = servo_list[SERVO_RIGHT].forward_angle;
		set_motor_speed(SPEED_FULL, 0);
	} else if (kinematics_mode.change_mode_counter == 20) {
		if (kinematics_mode.in_search == 1 && kinematics_mode.search_step == SEARCH_STEP_5_REWIND) {
			set_motor_speed(SEARCH_BACK_SPEED, 1);
			return 1;
		}	
	} else if (kinematics_mode.change_mode_counter == 30) {
		if (kinematics_mode.kinematics_speed == 0) {
			servo_list[SERVO_LEFT].speed = 4;		
		}		

		
	} else if (kinematics_mode.change_mode_counter == 100) {
		pidReset(servo_list[SERVO_LEFT].forward_angle);
		kinematics_mode.tension_sensor_enable = 1;
		return 1;
	}
	return 0;
}

static uint8_t rewind_timer()
{
	if (kinematics_mode.change_mode_counter == 0) {
		set_motor_speed(BRAKE_SPEED, 1);

		if (kinematics_mode.kinematics_speed == 0) {
			servo_list[SERVO_RIGHT].speed  = 2;
			servo_list[SERVO_REWIND].speed = 2;
		} else {
			servo_list[SERVO_RIGHT].speed  = 1;
			servo_list[SERVO_REWIND].speed = 1;
		}
		servo_list[SERVO_LEFT].speed = 1;
		servo_list[SERVO_PLAY].speed = 1;
		servo_list[SERVO_RIGHT].need_angle  = servo_list[SERVO_RIGHT].rewind_angle;
		servo_list[SERVO_PLAY].need_angle   = servo_list[SERVO_PLAY].rewind_angle;

	} else if (kinematics_mode.change_mode_counter == 10) {
		servo_list[SERVO_LEFT].need_angle   = servo_list[SERVO_LEFT].rewind_angle;
		servo_list[SERVO_REWIND].need_angle = servo_list[SERVO_REWIND].rewind_angle;
		set_motor_speed(SPEED_FULL, 0);
	} else if (kinematics_mode.change_mode_counter == 20) {
		if (kinematics_mode.in_search == 1 && kinematics_mode.search_step == SEARCH_STEP_5_REWIND) {
			set_motor_speed(SEARCH_BACK_SPEED, 1);
			return 1;
		}
	} else if (kinematics_mode.change_mode_counter == 60) {
		if (kinematics_mode.kinematics_speed == 0) {
			servo_list[SERVO_RIGHT].speed = 4;
		}		
		

		
	} else if (kinematics_mode.change_mode_counter == 100) {
		pidReset(servo_list[SERVO_RIGHT].rewind_angle);
		kinematics_mode.tension_sensor_enable = 1;
		return 1;
	}
	return 0;
}

static uint8_t stop_timer(uint8_t long_wait)
{
	if (kinematics_mode.change_mode_counter == 0) {
		if (kinematics_mode.kinematics_speed == 0) {
			servo_list[SERVO_RIGHT].speed  = 2;
			servo_list[SERVO_LEFT].speed   = 2;
			servo_list[SERVO_PLAY].speed   = 2;
			servo_list[SERVO_REWIND].speed = 2; 
		} else {
			servo_list[SERVO_RIGHT].speed  = 1;
			servo_list[SERVO_LEFT].speed   = 1;
			servo_list[SERVO_PLAY].speed   = 1;
			servo_list[SERVO_REWIND].speed = 1;
		}
				
		kinematics_mode.tension_sensor_enable = 0;
		
		if (kinematics_mode.previous == FORWARD_MODE || kinematics_mode.previous == FORWARD_SEARCH_MODE) {
			servo_list[SERVO_LEFT].need_angle = servo_list[SERVO_LEFT].stop_angle;
		} else if (kinematics_mode.previous == REWIND_MODE || kinematics_mode.previous == REWIND_SEARCH_MODE) {
			servo_list[SERVO_RIGHT].need_angle = servo_list[SERVO_RIGHT].min_angle; //1250;
		}
		
	} else if (kinematics_mode.change_mode_counter == 10) {
		set_motor_speed(BRAKE_SPEED, 2);
	} else if (kinematics_mode.change_mode_counter == 40) {	
		servo_list[SERVO_REWIND].need_angle = servo_list[SERVO_REWIND].stop_angle;
	} else if (kinematics_mode.change_mode_counter == 60) {
		servo_list[SERVO_PLAY].need_angle   = servo_list[SERVO_PLAY].stop_angle;
		servo_list[SERVO_LEFT].need_angle  = servo_list[SERVO_LEFT].stop_angle;
		servo_list[SERVO_RIGHT].need_angle = servo_list[SERVO_RIGHT].stop_angle;
		servo_list[SERVO_PLAY].need_angle   = servo_list[SERVO_PLAY].stop_angle;
		set_motor_speed(STOP_SPEED, 1);
		
	} else if (kinematics_mode.change_mode_counter == 80 && long_wait == 0) {
		return 1;
	} else if (kinematics_mode.change_mode_counter == 110 && long_wait == 1) {
		return 1;
	}

	return 0;
}

static uint8_t little_forward_timer()
{
	if (kinematics_mode.change_mode_counter == 0) {
		kinematics_mode.tension_sensor_enable = 0;
		set_motor_speed(400, 1);
	} else if (kinematics_mode.change_mode_counter == 50) {
		servo_list[SERVO_PLAY].need_angle   = servo_list[SERVO_PLAY].stop_angle+40;
	} else if (kinematics_mode.change_mode_counter == 53) {
		servo_list[SERVO_RIGHT].need_angle  = servo_list[SERVO_RIGHT].forward_angle;
		//servo_list[SERVO_LEFT].need_angle  = servo_list[SERVO_LEFT].forward_angle;
		//pidReset(servo_list[SERVO_RIGHT].forward_angle);
	} else if (kinematics_mode.change_mode_counter == 56) {
		servo_list[SERVO_REWIND].need_angle   = servo_list[SERVO_REWIND].forward_angle;
	} else if (kinematics_mode.change_mode_counter == 200) {
			kinematics_mode.tension_sensor_enable = 1;

		return 1;
	}
	
	return 0;
}

static uint8_t little_forward_timer_off()
{
	if (kinematics_mode.change_mode_counter == 0) {
		set_motor_speed(1000 + kinematics_mode.motor_speed_play_correction, 1);
		servo_list[SERVO_REWIND].need_angle  = servo_list[SERVO_REWIND].play_angle;
		servo_list[SERVO_LEFT].need_angle  = servo_list[SERVO_LEFT].stop_angle;
		servo_list[SERVO_RIGHT].need_angle  = servo_list[SERVO_RIGHT].play_angle;

	} else if (kinematics_mode.change_mode_counter == 50) {
		servo_list[SERVO_PLAY].need_angle   = servo_list[SERVO_PLAY].play_angle;

	} else if (kinematics_mode.change_mode_counter == 100) {
		kinematics_mode.tension_sensor_enable = 1;
		return 1;
	}
	
	return 0;
}


static uint8_t little_rewind_timer()
{
	if (kinematics_mode.change_mode_counter == 0) {
		kinematics_mode.servo_left_saved_angle = servo_list[SERVO_RIGHT].current_angle;
		kinematics_mode.tension_sensor_enable = 0;
		servo_list[SERVO_RIGHT].need_angle  = servo_list[SERVO_RIGHT].rewind_angle;
		pidReset(servo_list[SERVO_RIGHT].rewind_angle);
		set_motor_speed(400, 1);

	} else if (kinematics_mode.change_mode_counter == 60) {
		servo_list[SERVO_PLAY].need_angle   = servo_list[SERVO_PLAY].stop_angle+40;
	} else if (kinematics_mode.change_mode_counter == 73) {

		servo_list[SERVO_LEFT].need_angle  = servo_list[SERVO_LEFT].rewind_angle;
	} else if (kinematics_mode.change_mode_counter == 86) {
		servo_list[SERVO_REWIND].need_angle   = servo_list[SERVO_REWIND].rewind_angle;
		//kinematics_mode.tension_sensor_enable = 1;

		return 0;
	} else if (kinematics_mode.change_mode_counter == 200) {
		kinematics_mode.tension_sensor_enable = 1;

		return 1;
	}
	
	return 0;
}

static uint8_t little_rweind_timer_off()
{
	if (kinematics_mode.change_mode_counter == 0) {
		servo_list[SERVO_RIGHT].need_angle = servo_list[SERVO_RIGHT].min_angle; //1250;

	} else if (kinematics_mode.change_mode_counter == 30) {

		servo_list[SERVO_REWIND].need_angle  = servo_list[SERVO_REWIND].play_angle;


	} else if (kinematics_mode.change_mode_counter == 60) {
		servo_list[SERVO_LEFT].need_angle  = kinematics_mode.servo_left_saved_angle;

		pidReset(kinematics_mode.servo_left_saved_angle);
		
		servo_list[SERVO_PLAY].need_angle   = servo_list[SERVO_PLAY].play_angle;
		set_motor_speed(1000 + kinematics_mode.motor_speed_play_correction, 1);

	} else if (kinematics_mode.change_mode_counter == 100) {
		kinematics_mode.tension_sensor_enable = 1;
		return 1;
	}
	
	return 0;
}