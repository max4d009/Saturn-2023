/*
 * tension.h
 *
 * ? Created: 13.01.2023 1:59:18
 *  Author: m4d
 */ 
#ifndef TENSION_H_
#define TENSION_H_

#include "main.h"

#define TENSION_MIDDLE_ADC 450 // —редн€€ точка датчика нат€жени€. «начение ADC 450, чем больше тем меньше нат€жение


#define TENSION_PLAY_T 1
#define TENSION_FORWARD_T 1
#define TENSION_REWIND_T 1

#define PID_REGULATOR_TENSION_PLAY 0
#define PID_REGULATOR_TENSION_FORWARD 1
#define PID_REGULATOR_TENSION_REWIND 2

#define CONFIG_PID_REGULATOR_P 0
#define CONFIG_PID_REGULATOR_I 1
#define CONFIG_PID_REGULATOR_D 2

#define NUM_PID_REGULATOR 3
#define NUM_PID_REGULATOR_PARAM 3


#define SERVO_MIN_ANGLE   0
#define SERVO_MAX_ANGLE   160
#define SERVO_OFFSET      60
#define SPEED_SCALE       2
#define UPDATE_INTERVAL   6
#define STABLE_THRESHOLD  5

void tension_sensor_set_timer();
void tension_play_right_reel(uint8_t speed_left, uint8_t speed_right, uint8_t speed_sum);
void tension_init();

struct PidRegulator {
	volatile uint8_t p;
	volatile uint8_t i;
	volatile uint8_t d;
};
struct PidRegulatorCalculated {
	volatile double p;
	volatile double i;
	volatile double d;
};
struct PidRegulator pid_regulator_list[3];
struct PidRegulatorCalculated pid_regulator_calculated_list[3];


#endif /* TENSION_H_ */