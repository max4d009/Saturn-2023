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

void process_ir();
void while_ir();
void init_ir();
void ir_binding();

struct IRButton {
	PGM_P name;
	uint8_t eeprom_address;
	uint8_t code;
};

volatile struct IRButton ir_button_list[IR_BUTTONS_COUNT];

volatile unsigned char IR_impulse_count;
volatile unsigned char IR_wait;
uint8_t get_ir_command_ready();
void reset_ir_command_ready();
#endif /* IR_CONTR_H_ */