/*
 * servo_core.h
 *
 * λ Created: 09.07.2024 18:44:56
 *  Author: max4d
 */ 
#ifndef SERVO_CORE_H_
#define SERVO_CORE_H_

#include "main.h"

#define NUM_SERVO 4

// Пины для сервоприводов
#define SERVO_LEFT_PIN PB1
#define REWIND_SERVO_PIN PB2
#define PLAY_SERVO_PIN PB3
#define SERVO_RIGHT_PIN PB4

// Пин выключения питания на сервоприводы
#define SERVO_ENABLE_PORT PORTD
#define SERVO_ENABLE_DDR DDRD
#define SERVO_ENABLE_PIN PD3

#define SERVO_PORT PORTB
#define SERVO_DDR DDRB

#define TEST_KEYB_ADC_PIN PC0
#define TEST_KEYB_INTERRUPT_PIN PD2

#define REEL_LEFT_SENSOR_DDR DDRD
#define REEL_RIGHT_SENSOR_DDR DDRB
#define REEL_LEFT_SENSOR_PORT PORTD
#define REEL_RIGHT_SENSOR_PORT PORTB
#define REEL_LEFT_SENSOR_PIN PD5
#define REEL_RIGHT_SENSOR_PIN PB0

#define SERVO_0 0
#define SERVO_1 1
#define SERVO_2 2
#define SERVO_3 3

#define CYCLE 5000

#define SERVO_LEFT SERVO_0
#define SERVO_REWIND SERVO_1
#define SERVO_PLAY SERVO_2
#define SERVO_RIGHT SERVO_3

#define CONFIG_SERVO_MIN 0
#define CONFIG_SERVO_MAX 1
#define CONFIG_SERVO_PLAY 2
#define CONFIG_SERVO_STOP 3
#define CONFIG_SERVO_FORWARD 4
#define CONFIG_SERVO_REWIND 5
#define CONFIG_SERVO_PAUSE 6
#define CONFIG_SERVO_SEARCH 7
#define CONFIG_SERVO_SPEED 8

#define CONFIG_SERVO_COUNT_ALL 9

#define SEARCH_SPEED 600
#define SPEED_FULL 1300
#define SEARCH_BACK_SPEED 400
#define STOP_SPEED 200
#define BRAKE_SPEED 0
#define REEL_END_SPEED 330

#define AUTOSTOP_ADC_ACTIVATION 16
#define AUTOSTOP_OPACITY_TIMER 1

// Общая структура для всех сервоприводов
struct Servo {
	char pin;
	volatile uint8_t speed;
	volatile uint8_t max_angle;
	volatile uint8_t min_angle;
	volatile uint8_t current_angle;
	volatile uint8_t need_angle;
	volatile uint8_t play_angle;
	volatile uint8_t stop_angle;
	volatile uint8_t forward_angle;
	volatile uint8_t rewind_angle;
	volatile uint8_t pause_angle;
	volatile uint8_t search_angle;
	volatile uint8_t light_brake_angle;
	
};

struct KinematicsMode {
	uint8_t current;
	uint8_t previous;
	uint8_t in_process;
	uint8_t slow_rew;
	uint8_t in_search;
	uint8_t search_step;
	uint16_t tension;
	uint8_t tension_sensor_enable;
	uint8_t little_rewind_enable;
	uint16_t current_motor_speed;
	uint16_t need_motor_speed;
	uint16_t change_mode_counter;
	uint8_t motor_speed_play_correction;
	uint8_t kinematics_speed;
	uint8_t repeat;
	uint8_t autostop_activated;
	uint8_t autostop_observer_enabled;
	uint8_t tension_calibrate_enable;
	uint16_t servo_left_saved_angle;
	uint8_t reel_size;
};

volatile struct KinematicsMode kinematics_mode;

struct ReelsSpeed {
	uint8_t left_timer;
	uint8_t right_timer;
	uint8_t left;
	uint8_t right;
	uint16_t sum;
};

volatile struct ReelsSpeed reels_speed;

struct ProgramTimer {
	uint8_t div_x2;
	uint8_t div_x3;
	uint8_t div_x4;
	uint8_t div_x6;
	uint8_t div_x8;
	uint8_t div_x10;
	uint8_t div_x20;
	uint8_t div_x60;
};

struct ProgramTimer servo_timer;

struct Servo servo_list[NUM_SERVO];


void servo_update_timer();

#endif /* SERVO_CORE_H_ */