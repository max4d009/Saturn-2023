/*
 * pageServoConfig.h
 *
 * λ Created: 29.11.2023 3:36:09
 *  Author: max-4d
 */ 
#ifndef PAGESERVOCONFIG_H_
#define PAGESERVOCONFIG_H_

#include "../main.h"

#define NUM_SERVO 4

#define SERVO_LEFT 0
#define SERVO_REWIND 1
#define SERVO_PLAY 2
#define SERVO_RIGHT 3

#define CONFIG_SERVO_MIN 0
#define CONFIG_SERVO_MAX 1
#define CONFIG_SERVO_PLAY 2
#define CONFIG_SERVO_STOP 3
#define CONFIG_SERVO_FORWARD 4
#define CONFIG_SERVO_REWIND 5
#define CONFIG_SERVO_PAUSE 6
#define CONFIG_SERVO_SEARCH 7

#define MIN 0
#define MAX 250

#define MENU_SERVO_CONFIG_COUNT_PARAMS 8

void init_page_servo_config();
void page_servo_config_render(uint8_t first_render);
void page_servo_config_menu();
void page_servo_config_select();
void page_servo_config_plus();
void page_servo_config_minus();
void page_servo_config_save();

#endif /* PAGESERVOCONFIG_H_ */