/*
 * m4d_i2c.c
 *
 * Created: 17.12.2020 0:42:18
 *  Author: m4d
 */ 

#include "m4d_i2c.h"
	

// ������������ � ���� �������� ����������
void m4d_i2c_init_as_slave(void)
{
	TWAR = ME_ADDR & 0xFE;
	TWCR =
	(1 << TWIE)| // ��������� �������������
	(1 << TWEA)|
	(1 << TWEN) // ��������� ����������
	;
}


unsigned char transactType = 0;
unsigned char transactProcess = 0;
unsigned char transactCounter = 0;
unsigned char transactData[5];

// ������������ � ���� �������� ����������
void m4d_i2c_init_as_master(void)
{
	// �������� �������� (��� 8 ��� ���������� 100 ���)
	TWBR = 0x80;
}

void resetTWEN()
{
	TWCR |= (1 << TWEN);
	sei();
}

void executeCommand()
{
	switch (transactType){

		case I2C_START_TRANSACTION_SYMBOL_SPEED:
		//USART_send_int_var("t1", transactData[0]);
			//m4d_i2c_send_debug_int_var_oled("Speed", transactData[0]);
			set_target_speed((transactData[0] * 10), 0);
		break;
		
		case I2C_START_TRANSACTION_SYMBOL_SPEED_SAVE:
		//USART_send_int_var("t2", transactData[0]);
			//m4d_i2c_send_debug_int_var_oled("Speed", transactData[0]);
			set_target_speed((transactData[0]*10), 1);
		break;

		case I2C_START_TRANSACTION_SYMBOL_SPEED_FROM_EEPROM:
			//set_target_speed_from_eeprom();
		break;
		
		case I2C_START_TRANSACTION_SYMBOL_P_SAVE:
			set_p(transactData[0]);
		break;
		
		case I2C_START_TRANSACTION_SYMBOL_I_SAVE:
			set_i(transactData[0]);
		break;
		
		case I2C_START_TRANSACTION_SYMBOL_D_SAVE:
			set_d(transactData[0]);
		break;				
	}
}

// ������� sda �� �������� � ������ ��� �������, ��� �� scl �������
int m4d_i2c_start_condition(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	return i2c_wait_twin();
	// �������� ���� ����������� TWIN
	// ���������� ���������� � ����� ���� �� ����� ����������
}
void m4d_i2c_stop_condition(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

int i2c_wait_twin()
{
	unsigned int stop = 0;
	while (!(TWCR&(1 << TWINT))){
		stop++;
		if(stop == 2000){
			m4d_i2c_stop_condition();
			return 0;
		}
	}
	return 1;
}
// ��������� ���� ���������� �������
int m4d_i2c_do_send_byte(unsigned char c)
{
	TWDR = c; // ������� ���� � ������� ������
	TWCR = (1 << TWINT) | (1 << TWEN); // ������� �������� ������
	return i2c_wait_twin();
}

void m4d_i2c_send_debug_var_oled(char *varName, char *varValue)
{
	//cli();
	if(!m4d_i2c_start_condition()){sei(); return;}
	if(!m4d_i2c_do_send_byte(SLA_W_OLED)){sei(); return;}
	if(!m4d_i2c_do_send_byte(I2C_OLED_START_TRANSACTION_SYMBOL_DEBUG)){sei(); return;}
	unsigned char i = 0;
	while(varName[i]){
		if(!m4d_i2c_do_send_byte(varName[i])){sei(); return;}
		i++;
	}
	if(!m4d_i2c_do_send_byte(':')){sei(); return;}
	i++;
	if(!m4d_i2c_do_send_byte(' ')){sei(); return;}
	i++;
	unsigned char g = 0;
	while(varValue[g] && ((g+i) < I2C_DEBUG_STR_MAX_LEN)){
		if(!m4d_i2c_do_send_byte(varValue[g])){sei(); return;}
		g++;
	}
	
	if(!m4d_i2c_do_send_byte(I2C_OLED_STOP_TRANSACTION_SYMBOL)){sei(); return;}
	m4d_i2c_stop_condition();
	sei();
}
void m4d_i2c_send_debug_int_var_oled(char *varName, int varValue)
{
	char str[15];
	itoa(varValue, str, 10);
	m4d_i2c_send_debug_var_oled(varName, str);
}

