/*
 * time_search.c
 *
 * Created: 24.01.2025 2:33:15
 *  Author: max4d
 */ 
#include "time_search.h"

static int8_t step_search = -1;
static int16_t search_target_time = 0;
static int16_t search_real_target_time = 0;

int16_t timer_search_get_target_time()
{
	return search_target_time;
}

void timer_search_inc_target_time()
{
	if (search_target_time < 40*60) {
		search_target_time = search_target_time + 10;
	}
}

void timer_search_dec_target_time()
{
	if (search_target_time > -40*60) {
		search_target_time = search_target_time - 10;
	}
}

void timer_search_start_search()
{
	if (search_target_time != 0) {
		search_real_target_time = timer.all_sec + search_target_time;
		
		if (search_target_time > 0) {
			set_mode(FORWARD_MODE);
			step_search = STEP_0_SET_SEARCH_SPEED;
		} else if (search_target_time < 0) {
			set_mode(REWIND_MODE);
			step_search = STEP_0_SET_SEARCH_SPEED;
		}
	}
}

void timer_search_timer()
{
	static uint8_t wait = 0;
	static uint8_t motor_speed = 0;
	static uint8_t stop_coef = 1;
	uint8_t end_speed;
	
	if (step_search == STEP_OFF) {
		wait = 0;
		motor_speed = 0;
		stop_coef = 1;
	}

	switch (step_search) {
		case STEP_0_SET_SEARCH_SPEED:
			i2c_set_motor_speed(SEARCH_START_SPEED, 1);
			step_search = STEP_1_SET_MIDDLE_SPEED;
			if (
				(current.servo_real_mode == FORWARD_MODE && (search_real_target_time - timer.all_sec) <= SEARCH_SECONDS_BEFORE_SET_MIDDLE_SPEED) ||
				(current.servo_real_mode == REWIND_MODE && (timer.all_sec - search_real_target_time <= SEARCH_SECONDS_BEFORE_SET_MIDDLE_SPEED))
			) {
				wait = 5;
			}
		break;
		case STEP_1_SET_MIDDLE_SPEED:
			if (
				(current.servo_real_mode == FORWARD_MODE && (search_real_target_time - timer.all_sec) <= SEARCH_SECONDS_BEFORE_SET_MIDDLE_SPEED) ||
				(current.servo_real_mode == REWIND_MODE && (timer.all_sec - search_real_target_time <= SEARCH_SECONDS_BEFORE_SET_MIDDLE_SPEED))
			) {
				motor_speed = SEARCH_LOW_SPEED;
				i2c_set_motor_speed(motor_speed, 1);
				step_search = STEP_2_WAIT_MIDDLE_SPEED_STABILIZE;
			}
		break;
		case STEP_2_WAIT_MIDDLE_SPEED_STABILIZE:
			wait++;
			if (wait > 5) {
				step_search = STEP_3_SET_LOW_SPEED;
			}
		break;
		case STEP_3_SET_LOW_SPEED:
			end_speed = SEARCH_END_SPEED;
			if (motor_speed > end_speed) {
				if (
					(current.servo_real_mode == FORWARD_MODE && current.reel_speed_left > current.reel_speed_right * 1.5) ||
					(current.servo_real_mode == REWIND_MODE && current.reel_speed_right > current.reel_speed_left * 1.5)
				) {
					stop_coef = 2;
					end_speed = SEARCH_END_SPEED-10;
				}
				
				motor_speed--;
				motor_speed--;
				i2c_set_motor_speed(motor_speed, 1);
				break;
			}
			step_search = STEP_4_END;
		break;
		case STEP_4_END:
			if (
				(current.servo_real_mode == FORWARD_MODE && (timer.all_sec+stop_coef) >= search_real_target_time) ||
				(current.servo_real_mode == REWIND_MODE && (timer.all_sec-stop_coef) <= search_real_target_time)
			) {
				set_mode(STOP_MODE);
				timer_search_reset();
			}
		break;
	}
}

void timer_search_reset()
{
	search_real_target_time = 0;
	search_target_time = 0;
	step_search = STEP_OFF;
}
