/*
 * m4d_i2c.c
 *
 * λ Created: 17.12.2020 0:42:18
 *  Author: m4d
 */ 

#include "m4d_i2c.h"

static void send_ack();

volatile uint8_t need_execute = 0;

void m4d_i2c_init_as_slave(void)
{
	TWAR = ME_ADDR & 0xFE;
	TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWINT)|(1 << TWIE);
}

volatile uint8_t transactType = 0;
volatile uint8_t transactProcess = 0;
volatile uint8_t transactCounter = 0;
volatile char transactData[10];
volatile unsigned char status;
volatile uint8_t i2c_data[I2C_DATA_COUNT + 1];
volatile uint8_t i2c_data_current = 0;

static void send_ack()
{
	TWCR |= (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
}

static void send_n_ack()
{
	TWCR &= ~(1 << TWEA); TWCR |= (1 << TWINT) | (1 << TWEN);
}


ISR(TWI_vect)
{
	uint8_t send_count = I2C_DATA_COUNT;
	status = TWSR & 0xF8;
	
	if (status == 0x80) {
		char data = TWDR;
		if (transactProcess == 0) {
			transactType = data;
			transactProcess = 1;
		} else {
			transactData[transactCounter] = data;
			transactCounter++;
		}
		send_ack();
		return;
	} else if (status == 0xA0) {
		transactProcess = 0;
		transactCounter = 0;
		need_execute = 1;
		send_ack();
		return;
	} else if (status == 0xB0) {
		i2c_data_current = 0;
		return;
	} else if (status == 0xA8) {
		i2c_data_current = 0;
		if (i2c_data_current == send_count) {
			send_n_ack();
			i2c_data_current = 0;
		} else {
			send_ack();
		}
		return;
	} else if (status == 0xB8) {
		TWDR = i2c_data[i2c_data_current];
		i2c_data_current++;
		if (i2c_data_current == send_count) {
			send_n_ack();
			i2c_data_current = 0;
		} else {
			send_ack();
		}
		return;
	}
	send_ack();
}

uint8_t execute_command_timer()
{
	if (need_execute == 0) {
		return 0;
	}
	switch (transactType) {
		case I2C_GEN_START_TRANSACTION_SYMBOL_FREQ:
			if(transactData[0] <= 20){
				set_freq(transactData[0]*50);
			} else {
				set_freq(transactData[0]*100);
			}
		break;
	}
	need_execute = 0;
	return 1;
}
