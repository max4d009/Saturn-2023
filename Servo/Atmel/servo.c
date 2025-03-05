/*
 * m4d_servo.c
 *
 * λ Created: 04.12.2020 1:10:36
 *  Author: m4d
 */ 
#include "servo.h"

static void reel_end_timer(uint8_t speed_left, uint8_t speed_right, uint16_t speed_sum);

void reels_speed_timer(uint8_t speed_left, uint8_t speed_right, uint16_t speed_sum)
{ 
	if (kinematics_mode.current == PLAY_MODE) {
		tension_play_right_reel(speed_left, speed_right);
		return;
	}
	
	if (kinematics_mode.current != FORWARD_MODE && kinematics_mode.current != REWIND_MODE) {
		return;
	}
	
	reel_end_timer(speed_left, speed_right, speed_sum);
}

static void reel_end_timer(uint8_t speed_left, uint8_t speed_right, uint16_t speed_sum)
{
	static uint8_t timer = 0;
	
	if (kinematics_mode.slow_rew == 0 && speed_sum > 5 && timer < 10) {
		uint8_t coef = 2.6;
		if (speed_sum < 70) {
			coef = 3.2;
		}
		
		if (kinematics_mode.current == FORWARD_MODE && speed_left > speed_right * coef) {
			timer++;
		} else if (kinematics_mode.current == REWIND_MODE && speed_right > speed_left * coef) {
			timer++;
		} else {
			timer = 0;
		}
	} else {
		timer = 0;
	}
	
	if (timer == 4) {
		kinematics_mode.slow_rew = 1;
		usart_send_speed(REEL_END_SPEED, 0);
		timer = 0;
	}
}

void auto_stop_timer(uint16_t adc)
{
	static uint16_t blank_tape_wait = 0;
	static uint8_t reset_blank_tape_wait = 0;
		
	if (kinematics_mode.repeat > 0 && kinematics_mode.current == PLAY_MODE && audio_level.left > 20 && audio_level.right > 20) {
		blank_tape_wait++;
		if (blank_tape_wait > 300) {
			blank_tape_wait = 0;
			set_mode(STOP_MODE, 1, 1);
			return;
		}
	} else {
		reset_blank_tape_wait++;
		if (reset_blank_tape_wait > 3) {
			reset_blank_tape_wait = 0;
			blank_tape_wait = 0;
		}
	}
	
		
	if (kinematics_mode.current == STOP_MODE || kinematics_mode.autostop_activated == 1 || kinematics_mode.in_process == 1) {
		return;
	}
	
	static uint8_t after_black = 0;
	static uint8_t timer = 0;

	if (kinematics_mode.current == STOP_MODE) {
		after_black = 0;
		timer = 0;
		return;
	} else if (adc < AUTOSTOP_ADC_ACTIVATION) { // 100 Срабатывает на черной ленте
		if (after_black < 50) {
			after_black++;
		}
		return;
	} else if (after_black < 50) { // Ещё не было ленты
		after_black = 0;
		return;
	} else if (timer < AUTOSTOP_OPACITY_TIMER) { // 2
		timer++;
		return;
	}
	
	after_black = 0;
	timer = 0;
		
	set_mode(STOP_MODE, 1, 1);
}

void set_motor_speed(uint16_t speed, uint8_t pid_type)
{
	usart_send_speed(speed, pid_type);
	kinematics_mode.current_motor_speed = speed;
}


void servo_consuption_observer_timer(uint16_t servo_consuption)
{
	static uint8_t servo_overload = 0;
	
	if (servo_consuption < 310) {
		if (servo_overload < 61) {
			servo_overload++;
		}
		if (servo_overload == 40) {
			set_mode(STOP_MODE, 1, 0);
		}
		if (servo_overload == 60) {
			SERVO_ENABLE_PORT &= ~(1 << SERVO_ENABLE_PIN);
			servo_overload++;
		}
	} else {
		servo_overload = 0;
	}
}