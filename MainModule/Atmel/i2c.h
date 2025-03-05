/*
 * i2c.h
 *
 * λ Created: 17.12.2020 1:03:28
 *  Author: m4d
 */ 
#ifndef I2C_H_
#define I2C_H_

#include "main.h"

void i2c_init_as_master(void);
void i2c_send_byte_servo(unsigned char value);
void i2c_send_option_motherboard( uint8_t option,  uint8_t optionValue);
void i2c_send_mode_motherboard( uint8_t mode_);
void i2c_send_freq_oled(uint8_t freq_name,  uint8_t freq_level);
void i2c_send_freq_gen(uint8_t freq);
void i2c_send_mode_servo(uint8_t mode_);
void i2c_timer(uint8_t module_addr, uint8_t sla_w_module, uint8_t sla_r_module);
void i2c_send_config_current_servo(uint8_t servo_num);
void i2c_send_position_servo(uint8_t servo, uint8_t position);
void i2c_set_motor_speed(uint8_t speed, uint8_t type);
void i2c_save_motor_speed();
void i2c_save_position_servo(uint8_t servo, uint8_t parameter, uint8_t value);
void i2c_set_debug_mode(uint8_t debug);
void i2c_set_kinematics_speed(uint8_t speed);
void i2c_set_reels_size(uint8_t size);
void i2c_send_repeat_servo(uint8_t repeat);
void i2c_send_vu_params(char transact_symbol, uint8_t value);
void send_from_query_timer();
void i2c_send_config_current_pid(uint8_t pid_regulator_num);
void i2c_send_pid_koef(uint8_t pid_regulator_id, uint8_t koef_id, uint8_t value);
void i2c_save_pid_koef(uint8_t pid_regulator_id, uint8_t koef_id, uint8_t value);

//#define ME_ADDR 0x5F // последний разрешение отклика на адрес

#define VU_ADDR 0x3E
#define SLA_R_VU VU_ADDR|0b00000001 // Макрос адреса + бит чтения
#define SLA_W_VU VU_ADDR&0b11111110 // Макрос адреса + бит записи

#define MAINBOARD_ADDR 0x5E // 94
#define SLA_R_MAINBOARD MAINBOARD_ADDR|0b00000001 // Макрос адреса + бит чтения
#define SLA_W_MAINBOARD MAINBOARD_ADDR&0b11111110 // Макрос адреса + бит записи

#define GEN_ADDR 0x4A 
#define SLA_R_GEN GEN_ADDR|0b00000001 // Макрос адреса + бит чтения
#define SLA_W_GEN GEN_ADDR&0b11111110 // Макрос адреса + бит записи

#define SERVO_ADDR 0x4D // 77
#define SLA_R_SERVO SERVO_ADDR|0b00000001 // Макрос адреса + бит чтения
#define SLA_W_SERVO SERVO_ADDR&0b11111110 // Макрос адреса + бит записи

#define I2C_GEN_START_TRANSACTION_SYMBOL_FREQ 1

#define I2C_VU_START_TRANSACTION_PEAKS_MODE 1
#define I2C_VU_START_TRANSACTION_PEAKS_MODE_SAVE 2
#define I2C_VU_START_TRANSACTION_PEAKS_TIME 3
#define I2C_VU_START_TRANSACTION_PEAKS_TIME_SAVE 4

#define I2C_MOTHERBOARD_NR_OPTION 0
#define I2C_MOTHERBOARD_EQ_OPTION 1
#define I2C_MOTHERBOARD_KONTR_OPTION 2
#define I2C_MOTHERBOARD_GEN_OPTION 3
#define I2C_MOTHERBOARD_SADP_OPTION 4
#define I2C_MOTHERBOARD_UZ_EQ_OPTION 5
#define I2C_MOTHERBOARD_FIX_LEVEL_OPTION 6
#define I2C_MOTHERBOARD_MUTE_OPTION 7

#define I2C_MOTHERBOARD_START_TRANSACTION_SYMBOL_MODE 1
#define I2C_MOTHERBOARD_START_TRANSACTION_SYMBOL_OPTION 2

#define I2C_SERVO_START_TRANSACTION_SYMBOL_MODE 1
#define I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_CURRENT_SERVO 2
#define I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_MOVE 3
#define I2C_START_TRANSACTION_SYMBOL_SET_MOTOR_SPEED 4
#define I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SAVE_PARAM 5
#define I2C_SERVO_START_TRANSACTION_SYMBOL_DEBUG_ENABLE 6
#define I2C_START_TRANSACTION_SYMBOL_SAVE_MOTOR_SPEED 7
#define I2C_SERVO_START_TRANSACTION_SYMBOL_SET_KINEMATICS_SPEED 8
#define I2C_SERVO_START_TRANSACTION_SYMBOL_REPEAT 9
#define I2C_SERVO_START_TRANSACTION_SYMBOL_SET_REELS_SIZE 10
#define I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_CURRENT_PID_REGULATOR 11
#define I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SAVE_PID_KOEF 12
#define I2C_SERVO_START_TRANSACTION_SYMBOL_CONFIG_SEND_PID_KOEF 13

// #define TW_MT_DATA_ASK 0x28 // Ведущий передал данные и ведомый подтвердил приём
#define TW_MR_DATA_ASK 0x50 // Ведущий принял данные и передал подтверждение
#define TW_MR_DATA_NASK 0x58 // Ведущий передал данные и ведомый подтвердил приём

#define TWINT_WAIT_TIMEOUT 250

#define I2C_DATA_VU_PEAKS_MODE 0
#define I2C_DATA_VU_PEAKS_TIME 1
#define I2C_DATA_VU_COUNT 2

#define I2C_DATA_REEL_SPEED_LEFT 0
#define I2C_DATA_REEL_SPEED_RIGHT 1
#define I2C_DATA_KINEMATICS_MODE_CURRENT 2
#define I2C_DATA_KINEMATICS_IN_PROCESS 3
#define I2C_DATA_REPEAT 4
#define I2C_DATA_TENSION 5
#define I2C_DATA_AUDIO_L 6
#define I2C_DATA_AUDIO_R 7

#define I2C_DATA_SERVO_PLAY_CURRENT_ANGLE 8
#define I2C_DATA_SERVO_REWIND_CURRENT_ANGLE 9
#define I2C_DATA_SERVO_LEFT_CURRENT_ANGLE 10
#define I2C_DATA_SERVO_RIGHT_CURRENT_ANGLE 11

#define I2C_DATA_SERVO_DEBUG_FROM 12
#define I2C_DATA_SERVO_DEBUG_TO 19

#define I2C_DATA_CONFIG_SERVO_MIN 20
#define I2C_DATA_CONFIG_SERVO_MAX 21
#define I2C_DATA_CONFIG_SERVO_PLAY 22
#define I2C_DATA_CONFIG_SERVO_STOP 23
#define I2C_DATA_CONFIG_SERVO_FORWARD 24
#define I2C_DATA_CONFIG_SERVO_REWIND 25
#define I2C_DATA_CONFIG_SERVO_PAUSE 26
#define I2C_DATA_CONFIG_SERVO_SEARCH 27
#define I2C_DATA_CONFIG_MOTOR_SPEED 28

#define I2C_DATA_CONFIG_TENSION_P 6
#define I2C_DATA_CONFIG_TENSION_I 7
#define I2C_DATA_CONFIG_TENSION_D 8

#define I2C_DATA_COUNT 5
#define I2C_DATA_DEBUG_1_COUNT 29
#define I2C_DATA_DEBUG_2_COUNT 9
#define I2C_DATA_ALL_COUNT 29

#define I2C_SEND_QUERY_COUNT 12

struct I2CReadByte {
	unsigned char byte;
	uint8_t error;
};

struct I2CReadByte i2c_byte_d;

//i2c_data_query[]

struct Query {
	uint8_t i2c_data[6];
	uint8_t sended;
	uint8_t long_wait;
	uint8_t count;
};

struct Query query_list[I2C_SEND_QUERY_COUNT];


volatile uint8_t i2c_data[I2C_DATA_ALL_COUNT + 1];

#endif /* I2C_H_ */