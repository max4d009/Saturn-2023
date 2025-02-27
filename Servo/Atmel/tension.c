/*
 * λ tension.c
 *
 * Created: 13.01.2023 1:58:47
 *  Author: m4d
 */ 
#include "tension.h"

static void stabilization_play();
static void stabilization_forward();
static void stabilization_rewind();

void tension_init()
{
	for (uint8_t i = 0; i < NUM_PID_REGULATOR; i++) {
		pid_regulator_list[i].p = get_pid_regulator_eeprom_val(i, CONFIG_PID_REGULATOR_P);
		pid_regulator_list[i].i = get_pid_regulator_eeprom_val(i, CONFIG_PID_REGULATOR_I);
		pid_regulator_list[i].d = get_pid_regulator_eeprom_val(i, CONFIG_PID_REGULATOR_D);
		
		pid_regulator_calculated_list[i].p = pid_regulator_list[i].p * 0.01;
		pid_regulator_calculated_list[i].i = pid_regulator_list[i].i * 0.01;
		pid_regulator_calculated_list[i].d = pid_regulator_list[i].d * 0.01;		
	}
}

void tension_sensor_set_timer()
{		
	switch (kinematics_mode.current) {
		case PLAY_MODE:
		case REC_MODE_PLAY:
			stabilization_play();
			return;
		case REWIND_MODE:
		case REWIND_SEARCH_MODE:
		case REWIND_LITTLE_MODE:		
			stabilization_rewind();
			return;
		case FORWARD_MODE:
		case FORWARD_SEARCH_MODE:
		case FORWARD_LITTLE_MODE:
			stabilization_forward();
			return;
	}
}

void tension_play_right_reel(uint8_t speed_left, uint8_t speed_right)
{	
	static uint8_t reel_right_stable_inc = 0;	
	static uint8_t rt = 0;
	static uint8_t rt_buf = 0;

	if (kinematics_mode.current != PLAY_MODE || kinematics_mode.tension_sensor_enable == 0 || reels_speed.sum < 3) {
		if (servo_list[SERVO_RIGHT].current_angle != servo_list[SERVO_RIGHT].play_angle) {
			servo_list[SERVO_RIGHT].need_angle = servo_list[SERVO_RIGHT].play_angle;
		}
		reel_right_stable_inc = 0;
		return;
	}
	
	if (rt_buf < 250) {
		rt_buf = rt_buf + speed_right;
	}
	
	rt++;
	
	if (rt > 6) {
		uint8_t res_angle = servo_list[SERVO_RIGHT].play_angle - 60 + rt_buf*2;
		if (res_angle > 160) {
			res_angle = 160;
		}
		
		if (servo_list[SERVO_RIGHT].current_angle < res_angle) {
			reel_right_stable_inc++;
			if (reel_right_stable_inc > 5) {
				servo_list[SERVO_RIGHT].need_angle = res_angle;
				reel_right_stable_inc = 0;
			}
		} else {
			reel_right_stable_inc = 0;
		}
		
		if (servo_list[SERVO_RIGHT].current_angle > res_angle) {
			servo_list[SERVO_RIGHT].need_angle = res_angle;
		}
		
		rt = 0;
		rt_buf = 0;
	}
}

static void stabilization_play()
{
	servo_list[SERVO_LEFT].need_angle = computePID(
		kinematics_mode.tension,
		TENSION_MIDDLE_ADC,
		pid_regulator_calculated_list[PID_REGULATOR_TENSION_PLAY].p,
		pid_regulator_calculated_list[PID_REGULATOR_TENSION_PLAY].i,
		pid_regulator_calculated_list[PID_REGULATOR_TENSION_PLAY].d,
		TENSION_PLAY_T,
		servo_list[SERVO_LEFT].min_angle,
		servo_list[SERVO_LEFT].max_angle
	);	
}
	
static void stabilization_forward()
{
	double kp = pid_regulator_calculated_list[PID_REGULATOR_TENSION_FORWARD].p;
	double ki = pid_regulator_calculated_list[PID_REGULATOR_TENSION_FORWARD].i;
	if (kinematics_mode.reel_size < 15) {
		kp = TENSION_FORWARD_P_SMALL_REEL;		
		ki = TENSION_FORWARD_I_SMALL_REEL;
	}

	servo_list[SERVO_LEFT].need_angle = computePID(
		kinematics_mode.tension,
		TENSION_MIDDLE_ADC,
		kp,
		ki,
		pid_regulator_calculated_list[PID_REGULATOR_TENSION_FORWARD].d,
		TENSION_FORWARD_T,
		servo_list[SERVO_LEFT].min_angle,
		servo_list[SERVO_LEFT].max_angle
	);
}
	
static void stabilization_rewind()
{
	double kp = pid_regulator_calculated_list[PID_REGULATOR_TENSION_REWIND].p;
	double ki = pid_regulator_calculated_list[PID_REGULATOR_TENSION_REWIND].i;
	if (kinematics_mode.reel_size < 15) {
		kp = TENSION_REWIND_P_SMALL_REEL;
		ki = TENSION_REWIND_I_SMALL_REEL;
	}
		i2c_send_debug_int_var_oled("R",kinematics_mode.tension);

	servo_list[SERVO_RIGHT].need_angle = computePID(
		kinematics_mode.tension,
		TENSION_MIDDLE_ADC,
		kp,
		ki,
		pid_regulator_calculated_list[PID_REGULATOR_TENSION_REWIND].d,
		TENSION_REWIND_T,
		servo_list[SERVO_RIGHT].min_angle,
		servo_list[SERVO_RIGHT].max_angle
	); 
}