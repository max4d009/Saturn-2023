/*
 * main.h
 *
 * λ Created: 06.11.2021 2:26:18
 *  Author: m4d
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <compat/twi.h>
#include <avr/interrupt.h>

#include "m4d_i2c.h"

#define RELAY_GEN_ID 0
#define RELAY_FIX_REC_ID 1
#define RELAY_KONTR_ID 2
#define RELAY_EQ_ID 3
#define RELAY_GSP_ID 4
#define RELAY_LINE_ID 5
#define RELAY_REC_ID 6

#define RELAY_GENERAL_PIN PD2

#define RELAY_GEN_PIN PD3
#define RELAY_FIX_REC_PIN PD4
#define RELAY_KONTR_PIN PD5
#define RELAY_EQ_PIN PD6
#define RELAY_GSP_PIN PD7
#define RELAY_LINE_PIN PB6
#define RELAY_REC_PIN PB7

#define OPTO_EQ_PIN PB5
#define OPTO_NR_PIN PC0
#define OPTO_GEN_PIN PC1
#define OPTO_UZ1_PIN PC2
#define OPTO_UZ0_PIN PC3
#define OPTO_SADP0_PIN PB0
#define OPTO_SADP1_PIN PB1
#define OPTO_SADP2_PIN PB2
#define OPTO_SADP3_PIN PB3

#define OPTOCOUPLER_EQ_ID 0
#define OPTOCOUPLER_NR_ID 1
#define OPTOCOUPLER_GEN_ID 2
#define OPTOCOUPLER_UZ1_ID 3
#define OPTOCOUPLER_UZ0_ID 4
#define OPTOCOUPLER_SADP0_ID 5
#define OPTOCOUPLER_SADP1_ID 6
#define OPTOCOUPLER_SADP2_ID 7
#define OPTOCOUPLER_SADP3_ID 8


#define RELAY_PINS_DDRD_COUNT 6
const int RELAY_PINS_DDRD[RELAY_PINS_DDRD_COUNT] = {
	RELAY_GENERAL_PIN,
	RELAY_GEN_PIN,
	RELAY_FIX_REC_PIN,
	RELAY_KONTR_PIN,
	RELAY_EQ_PIN,
	RELAY_GSP_PIN
};

#define RELAY_PINS_DDRB_COUNT 2
const int RELAY_PINS_DDRB[RELAY_PINS_DDRB_COUNT] = {
	RELAY_LINE_PIN,
	RELAY_REC_PIN
};


struct RelayPins {
	volatile uint8_t *port;
	unsigned int pin;
};

struct OptoPins {
	volatile uint8_t *port;
	unsigned int pin;
};


#define GEN_STATE_ON 0
#define GEN_STATE_OFF 1
#define EQ_STATE_ON 1
#define EQ_STATE_OFF 0
#define EQ_STATE_MIDDLE 2
#define GSP_STATE_ON 1
#define GSP_STATE_OFF 0
#define REC_STATE_ON 0
#define REC_STATE_OFF 1
#define KONTR_STATE_ON 0
#define KONTR_STATE_OFF 1
#define LINE_STATE_ON 1
#define LINE_STATE_OFF 0
#define FIX_STATE_ON 1
#define FIX_STATE_OFF 0
#define NR_STATE_ON 1
#define NR_STATE_OFF 0

#define REC_MODE 0
#define PLAY_MODE 1
#define STOP_MODE 2
#define REWIND_MODE 3
#define FORWARD_MODE 4
#define PAUSE_MODE 5
#define REWIND_SEARCH_MODE 6
#define FORWARD_SEARCH_MODE 7
#define REC_MODE_GEN 8
#define REC_MODE_PLAY 9

#define NR_OPTION 0
#define EQ_OPTION 1
#define KONTR_OPTION 2
#define GEN_OPTION 3
#define SADP_OPTION 4
#define UZ_EQ_OPTION 5
#define FIX_LEVEL_OPTION 6
#define MUTE_OPTION 7

#define TEST_PD0_PIN PD0
#define TEST_PD1_PIN PD1

void set_option(char option_, char value_);
void set_mode(uint8_t mode);



#endif /* MAIN_H_ */