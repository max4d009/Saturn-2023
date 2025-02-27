/*
 * λ tension.h
 *
 * Created: 13.01.2023 1:59:18
 *  Author: m4d
 */ 
#ifndef TENSION_H_
#define TENSION_H_

#include "main.h"

#define TENSION_MIDDLE_ADC 450 // Средняя точка датчика натяжения. Значение ADC 450, чем больше тем меньше натяжение

#define TENSION_PLAY_P 0.08
#define TENSION_PLAY_I 0.03
#define TENSION_PLAY_D 0.008
#define TENSION_PLAY_T 1

#define TENSION_FORWARD_P 5
#define TENSION_FORWARD_I 3
#define TENSION_FORWARD_D 0.8
#define TENSION_FORWARD_T 1
#define TENSION_FORWARD_P_SMALL_REEL 0.05
#define TENSION_FORWARD_I_SMALL_REEL 0.003

#define TENSION_REWIND_P 0.12
#define TENSION_REWIND_I 0.007
#define TENSION_REWIND_D 0.0004
#define TENSION_REWIND_T 1
#define TENSION_REWIND_P_SMALL_REEL 0.05
#define TENSION_REWIND_I_SMALL_REEL 0.003

#define PID_REGULATOR_TENSION_PLAY 0
#define PID_REGULATOR_TENSION_FORWARD 1
#define PID_REGULATOR_TENSION_REWIND 2

#define CONFIG_PID_REGULATOR_P 0
#define CONFIG_PID_REGULATOR_I 1
#define CONFIG_PID_REGULATOR_D 2

#define NUM_PID_REGULATOR 3
#define NUM_PID_REGULATOR_PARAM 3

void tension_sensor_set_timer();
void tension_play_right_reel(uint8_t speed_left, uint8_t speed_right);
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