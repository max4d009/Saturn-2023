/*****************************************************
Project: IRContr
Based on: https://forum.vegalab.ru/showthread.php?t=38671
*****************************************************/
#include "ir_contr.h"

static void ir_button_press(uint8_t button);

volatile uint8_t End_bit;                  // ���� ���������� ����� ����� ������� ��

volatile unsigned char IR_i;               // �������, ��������������� ��� ���������� ������ ��
volatile int system_code;                  // ��� ������� ��
uint16_t system_code_eepprom_address = 4335;
uint32_t system_code_saved = 0;

volatile unsigned char ir_code;                     // ������ ��

unsigned char i;
unsigned char ii = 0;
unsigned char button;
unsigned char t_pressed = 0; // ������� ������� ��������� ������
volatile uint8_t binding_mode;


void init_ir()
{
	ir_button_list[IR_BUTTON_OFF].name = PSTR("����");
	ir_button_list[IR_BUTTON_OFF].eeprom_address = 50;
	
	ir_button_list[IR_BUTTON_ON].name = PSTR("���");
	ir_button_list[IR_BUTTON_ON].eeprom_address = 51;
	
	ir_button_list[IR_BUTTON_KONTR].name = PSTR("�����");
	ir_button_list[IR_BUTTON_KONTR].eeprom_address = 52;
	
	ir_button_list[IR_BUTTON_EQ].name = PSTR("��");
	ir_button_list[IR_BUTTON_EQ].eeprom_address = 53;

	ir_button_list[IR_BUTTON_NR].name = PSTR("��");
	ir_button_list[IR_BUTTON_NR].eeprom_address = 54;
	
	ir_button_list[IR_BUTTON_STOP].name = PSTR("����");
	ir_button_list[IR_BUTTON_STOP].eeprom_address = 55;
	
	ir_button_list[IR_BUTTON_PLAY].name = PSTR("����");
	ir_button_list[IR_BUTTON_PLAY].eeprom_address = 56;
	
	ir_button_list[IR_BUTTON_REWIND].name = PSTR("���");
	ir_button_list[IR_BUTTON_REWIND].eeprom_address = 57;
	
	ir_button_list[IR_BUTTON_FORWARD].name = PSTR("����");
	ir_button_list[IR_BUTTON_FORWARD].eeprom_address = 58;
	
	ir_button_list[IR_BUTTON_PAUSE].name = PSTR("���");
	ir_button_list[IR_BUTTON_PAUSE].eeprom_address = 59;
	
	ir_button_list[IR_BUTTON_REPEAT].name = PSTR("����");
	ir_button_list[IR_BUTTON_REPEAT].eeprom_address = 60;
		
	// ������������ �������� ������ ������
	for (i = 0; i < IR_BUTTONS_COUNT; i++) {
		ir_button_list[i].code = EEPROM_read(ir_button_list[i].eeprom_address);
	}
	system_code_saved = EEPROM_read_dword(system_code_eepprom_address);
				//send_debug_int_var_oled("code", system_code_eep);
    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: 31.25 kHz
    // Mode: Normal top=FFh
    TCCR0B=0x04;

    // External Interrupt(s) initialization
    // INT0: On
    // INT0 Mode: Any change
    // INT1: Off	
    EICRA=0x01;
    EIMSK=0x01;
    EIFR=0x01;
	
	End_bit = 0;
	IR_wait = 0;
}

void ir_binding()
{
	binding_mode = 1;
	init_ir();
	char button_name[6];
	for (i = 0; i < IR_BUTTONS_COUNT; i++) {
		disp1color_FillScreenbuff(0);

		memset(button_name, ' ', 6);
		strcpy_P(button_name, ir_button_list[i].name);
		//oled_printf(0, 10, FONTID_6X8M, "��:%s", button_name);
		char str_buf[10];
		oled_printf(0, 10, FONTID_6X8M, strcpy_P(str_buf, PSTR("��:%s")), button_name);
		disp1color_UpdateFromBuff();
		
		sei();
		while (End_bit != 1);
		cli();
		if(system_code < 0) {
			system_code = system_code * -1;
		}
		
		EEPROM_write_dword(system_code_eepprom_address, system_code);
		EEPROM_write(ir_button_list[i].eeprom_address, ir_code);
		End_bit = 0;
		
		//oled_printf(0, 30, FONTID_6X8M, "s:%d", system_code);
		//oled_printf(0, 40, FONTID_6X8M, "c:%d", ir_code);
		disp1color_UpdateFromBuff();
		_delay_ms(400);
	}
	
 	binding_mode = 0;
 	IR_impulse_count = 0;
	init_ir();
	
	sei();
}

static void ir_button_press(uint8_t button)
{
	switch (button) {
		case IR_BUTTON_OFF:
			off();
			break;
		case IR_BUTTON_ON:
			on();
			break;
		case IR_BUTTON_KONTR:
			change_option(KONTR_OPTION);
			break;
		case IR_BUTTON_EQ:
			change_option(EQ_OPTION);
			break;
		case IR_BUTTON_NR:
			change_option(NR_OPTION);
			break;
		case IR_BUTTON_STOP:
			set_mode(STOP_MODE);
			break;
		case IR_BUTTON_PLAY:
			set_mode(PLAY_MODE);
			break;
		case IR_BUTTON_REWIND:
			set_mode(REWIND_MODE);
			break;
		case IR_BUTTON_FORWARD:
			set_mode(FORWARD_MODE);
			break;
		case IR_BUTTON_PAUSE:
			set_mode(PAUSE_MODE);
			break;
		case IR_BUTTON_REPEAT:
			//change_option(KONTR_OPTION);
			break;
	}
}

void while_ir()
{
    if (binding_mode == 1) {
		return;
	}
		
	if(system_code < 0) {
		system_code = system_code * -1;
	}	
		
	if (system_code == system_code_saved) {  // ���� ������� �������
		cli();
		for (uint8_t i = 0; i < IR_BUTTONS_COUNT; i++) {            // ������� ������� ������
			if (ir_code == ir_button_list[i].code) {
				ir_button_press(i);
				break;
			}
		}
	}
	
	system_code = 0xFFFE;   // ������� ��� ����������
	ir_code = 0xFE;         // ������� ������
	End_bit = 0;            // �������� ��� ����� �����
	EICRA = 0;              // ������������� ���������� �� ������� ������ �� INT0(�����������)
	EICRA = 0x01;           // any logical change on INT0 interrupt
			
	IR_impulse_count = 0;
	IR_wait = 0;
	sei();
}

// ���������� ���������� �� ������������
// ��� ��������� ������������ ������������ Timer0 Ft=8���/256 -> T=32us
// for NEC format
// 0       0.5625ms  14...22
// 1       1.6875ms  42...64
// kept pr 2.25ms    56...84
// start   4.5ms     112...170
ISR (INT0_vect)
{
	if (End_bit == 1) {
		TIFR0 |= (1<<TOV0); // ���������� ���� ������������ (������� ���. 1)
		return;
	}
	
	unsigned char tau = TCNT0;
	TCNT0 = 0; // �������� ������� �������

	// ���� �� ������ ������������ ����
	// ���� ������0 ������������ (����. "1" �� IRIn ���� ������ 8.192ms)
	// ����� ���� ���� ����� ������� ���� ��������� ������
	if (!(PIND & (1 << IR_PIN))) {
		if (TIFR0 & (1 << TOV0)) {
			IR_impulse_count = 0;     // �������� ������� ���-�� �������� ���������
		} else if ((IR_impulse_count == 0) && (tau > 55) && (tau < 85)) { // ���� ���������� ������ �������
			IR_i = 0;                 //��������� ������� ��� 130ms
		} else if ((IR_impulse_count > 0) && (((tau > 13) && (tau < 23)) || ((tau > 41) && (tau < 65)))) {  // ���� ������� �� ������ ������� � ��������. �������. 0 ��� 1
			if (IR_impulse_count == 32) {
				End_bit = 1;                        // ������������� ��� ����� ����� �� 32-� ��������
				while_ir();
			} else {
				if (IR_impulse_count < 17) {        // ��������� ��� �������
					system_code <<= 1;
					if (tau > 41) {
						system_code |= 1;           // ������� 1?
						IR_wait = 1;
					}
				} else if (IR_impulse_count < 25) { // ��������� ������
					ir_code <<= 1;
					if (tau > 41) {
						ir_code |= 1;                // ������� 1?
					}
				}
				IR_impulse_count++;
			}
		} else {                                    // ������������ ��� �������� ��� ��������� ������� 4.5ms
			system_code = 0xFFFE;
			ir_code = 0xFE;
			End_bit = 0;                            // �������� ��� ����� �����
			if ((IR_impulse_count == 0) && (tau>111) && (tau < 171)) {  //���� ������� ��������� �������
				IR_impulse_count = 1;
				IR_i = 0;                           // ���� ��� 130��
				//
			} else {
				IR_impulse_count = 0;
				
			}
		}
	}
	TIFR0 |= (1<<TOV0); // ���������� ���� ������������ (������� ���. 1)
}