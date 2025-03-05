/*
 * ir_contr.h
 *
 * λ Created: 07.12.2023 1:41:35
 *  Author: max-4d
 */ 
#ifndef IR_CONTR_H_
#define IR_CONTR_H_

#include "main.h"

#define IR_BUTTON_OFF 0
#define IR_BUTTON_ON 1
#define IR_BUTTON_KONTR 2
#define IR_BUTTON_EQ 3
#define IR_BUTTON_NR 4
#define IR_BUTTON_STOP 5
#define IR_BUTTON_PLAY 6
#define IR_BUTTON_REWIND 7
#define IR_BUTTON_FORWARD 8
#define IR_BUTTON_PAUSE 9
#define IR_BUTTON_REPEAT 10

#define IR_BUTTONS_COUNT 11

void while_ir();
void init_ir();
void ir_binding();

struct IRButton {
	PGM_P name;
	uint8_t eeprom_address;
	uint8_t code;
};

volatile struct IRButton ir_button_list[IR_BUTTONS_COUNT];

volatile unsigned char IR_impulse_count;   // счётчик кол-ва принятых импульсов ДУ
volatile unsigned char IR_wait;   // счётчик кол-ва принятых импульсов ДУ


#endif /* IR_CONTR_H_ */