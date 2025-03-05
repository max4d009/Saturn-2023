/*
 * i2c.c
 *
 * λ Created: 19.08.2024 1:32:32
 *  Author: max4d
 */ 
#include "i2c.h"

static void send_ack();
static void send_n_ack();
static void update_i2c_data_timer();
volatile unsigned char status;
volatile char transactData[10];
volatile uint8_t need_execute = 0;
volatile uint8_t i2c_data[I2C_DATA_COUNT + 1];
volatile uint8_t i2c_data_current = 0;
volatile uint8_t transactType = 0;
volatile uint8_t transactProcess = 0;
volatile uint8_t transactCounter = 0;

void i2c_init_as_slave(void)
{
	TWAR = ME_ADDR & 0xFE;
	TWCR=(1<<TWEN)|(1<<TWEA)|(1<<TWINT)|(1 << TWIE);/* Enable TWI, Enable ack generation */
	//while (!(TWCR & (1<<TWINT)));
}

ISR(TWI_vect)
{
	uint8_t send_count = I2C_DATA_COUNT;
	status = TWSR & 0xF8; 
	
		switch (status)
		{
			case 0x80:;  // Пришли данные от мастера. (До это мы уже приняли свой адрес)
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
			case 0xA0:;
				transactProcess = 0;
				transactCounter = 0;
				need_execute = 1;
				send_ack();
				break;
				//--- Status-Codes Slave Transmitter Modus
			case 0xa8: // own slave adress received
				update_i2c_data_timer();
				i2c_data_current = 0;
			case 0xb8: // byte was sent and ACK received

				TWDR = i2c_data[i2c_data_current];
				i2c_data_current++;
				if (i2c_data_current == send_count) {
					send_n_ack();
					} else {
					send_ack();
				}
			break;
			case 0xc0: // last byte was sent
			case 0xc8:
				//TWCR = 0xc5; // set TWEA
				send_ack();
			break;
				case 0x00: // BUS-Error
				TWCR = 0xd5;
			break;
			default:
			//send_ack();
			send_ack();
		}
}

static void send_ack()
{
	TWCR = (1<<TWEN) | (1<<TWIE) | (1<<TWINT) | (1<<TWEA);
}

static void send_n_ack()
{
	TWCR =  (1<<TWEN)|(1<<TWIE)|(1<<TWINT);
}

static void update_i2c_data_timer()
{
	i2c_data[I2C_DATA_PEAKS_MODE] = get_peaks_mode();
	i2c_data[I2C_DATA_PEAKS_TIME] = get_peaks_time();
}

uint8_t execute_command_timer()
{
	if (need_execute == 0) {
		return 0;
	}
	
	switch (transactType) {
		case I2C_VU_START_TRANSACTION_PEAKS_MODE:
			set_peaks_mode(transactData[0]);
		break;
		case I2C_VU_START_TRANSACTION_PEAKS_MODE_SAVE:
			EEPROM_write(PR_MODE_PEAKS_MODE, transactData[0]);
		break;
		case I2C_VU_START_TRANSACTION_PEAKS_TIME:
			set_peaks_time(transactData[0]);
		break;
		case I2C_VU_START_TRANSACTION_PEAKS_TIME_SAVE:
			EEPROM_write(PR_MODE_PEAKS_TIME, transactData[0]);
		break;		
	}
	
	need_execute = 0;
	return 1;
}
