/*
 * m4d_i2c.c
 *
 * Created: 17.12.2020 0:42:18
 *  Author: m4d
 */ 

#include "m4d_i2c.h"

volatile unsigned char status;
volatile uint8_t need_execute = 0;
volatile uint8_t i2c_data_current = 0;

// Инициализаця в роли ведомого устройства
void m4d_i2c_init_as_slave(void)
{
	TWAR = ME_ADDR & 0xFE;
	TWCR = (1 << TWEN)|(1 << TWEA)|(1 << TWINT)|(1 << TWIE);
	
	while (!(TWCR & (1<<TWINT)));
		
	// while(TWCR&(0xF8)!=(0x60));
}

char transactType = 0;
char transactProcess = 0;
uint8_t transactCounter = 0;
char transactData[3] = {' '};
	

static void send_ack()
{
	TWCR = (1<<TWEN) | (1<<TWIE) | (1<<TWINT) | (1<<TWEA);
}

static void send_n_ack()
{
	TWCR =  (1<<TWEN) | (1<<TWIE) | (1<<TWINT);
}

ISR(TWI_vect)
{
	status = TWSR & 0xF8;
	uint8_t send_count = 2;
	switch (status)
	{
		case 0x80:
		{  // Пришли данные от мастера. (До это мы уже приняли свой адрес)
			char data = TWDR;
			if (transactProcess == 0) {
				transactType = data;
				transactProcess = 1;
			} else {
				transactData[transactCounter] = data;
				transactCounter++;
			}
			send_ack();
			break;
		}
		case 0xA0:;
			transactProcess = 0;
			transactCounter = 0;
			need_execute = 1;
			send_ack();
		break;
		case 0xa8:
		{
			 i2c_data_current = 0;
		}
		case 0xb8:
		{
			TWDR = 0;
			i2c_data_current++;
			if (i2c_data_current == send_count) {
				send_n_ack();
			} else {
				send_ack();
			}
			break;
		}
		case 0xc0:
		{
			send_ack();
			break;
		}
		case 0xc8:{
			send_ack();
			break;
		}
		case 0x00:
		{
			TWCR = 0xd5;
			break;
		}
		default:
		{
			TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);  
			//send_ack();
		}
		
	}
}

uint8_t execute_command_timer()
{
	if (need_execute == 0) {
		return 0;
	}
	switch (transactType) {
		case I2C_MOTHERBOARD_START_TRANSACTION_SYMBOL_MODE:
			set_mode(transactData[0]);
		break;
		case I2C_MOTHERBOARD_START_TRANSACTION_SYMBOL_OPTION:
			set_option(transactData[0], transactData[1]);
		break;
	}
	need_execute = 0;
	return 1;
}