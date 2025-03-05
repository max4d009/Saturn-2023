/*
 * m4d_servo.h
 *
 * λ Created: 08.12.2020 1:27:25
 *  Author: m4d
 */ 
#ifndef M4D_SERVO_H_
#define M4D_SERVO_H_

#include "main.h"

void m4d_servo_init();
void auto_stop_timer(uint16_t adc);
void change_mode_timer();
void reels_speed_timer(uint8_t speed_left, uint8_t speed_right, uint16_t speed_sum);
void set_motor_speed(uint16_t speed, uint8_t pid_type);
void servo_consuption_observer_timer(uint16_t servo_consuption);

#endif /* M4D_SERVO_H_ */