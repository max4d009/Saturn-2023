/*
 * vu.c
 *
 * λ Created: 11.07.2021 23:28:18
 * Author : m4d
 */ 
#include "main.h"

static int8_t prevDiodLeft = 0;
static int8_t prevDiodRight = 0;
static uint16_t countLeft = 0;
static uint16_t countRight = 0;
static int8_t sat1 = 0, sat2 = 0, sat3 = 0, sat4 = 0;
static uint8_t peaks_mode = 0;
static uint16_t peaks_time = 0;
static unsigned char VPORT = 0b00000000;
static uint8_t right_vu_array[20] = {0};


static void sat_text_anim();
static void sat_left_text_anim();
static void soft_spi(uint8_t sat1, uint8_t sat2, uint8_t sat3, uint8_t vu);
static void sat_right_text_anim();
static uint16_t get_level_by_channel(uint8_t channel);

void set_peaks_mode(uint8_t mode)
{
	prevDiodLeft = 0;
	prevDiodRight = 0;
	soft_spi(sat1, sat2, sat3, 0x00);
	PORTD = 0x00;
	peaks_mode = mode;
}

uint8_t get_peaks_mode()
{
	return peaks_mode;
}

void set_peaks_time(uint8_t time)
{
	peaks_time = time * 100;
}

uint8_t get_peaks_time()
{
	return peaks_time / 100;
}

int8_t getNumDiodByAdc(uint16_t adc)
{
	if (adc > 870) {
		return 7;
	} else if (adc > 680) {
		return 6;
	} else if (adc > 570) {
		return 5;
	} else if (adc > 520) {
		return 4;
	} else if (adc > 440) {
		return 3;
	} else if (adc > 330) {
		return 2;
	} else if (adc > 220) {
		return 1;
	} else if (adc > 130) {
		return 0;
	}
	return -1;
}

static void soft_spi(uint8_t sat1, uint8_t sat2, uint8_t sat3, uint8_t vu)
{
	vu = (vu & 0xF0) >> 4 | (vu & 0x0F) << 4;
	vu = (vu & 0xCC) >> 2 | (vu & 0x33) << 2;
	vu = (vu & 0xAA) >> 1 | (vu & 0x55) << 1;
	
	sat3 = (sat3 & 0xF0) >> 4 | (sat3 & 0x0F) << 4;
	sat3 = (sat3 & 0xCC) >> 2 | (sat3 & 0x33) << 2;
	sat3 = (sat3 & 0xAA) >> 1 | (sat3 & 0x55) << 1;
	
	char k;
	PORT &= ~(1<<E);
	
	for (k = 0; k < 8; k++) {
		PORT &=~ (1 << CLK);
		_delay_ms(1);
		
		if (vu >> k & 1) {
			PORT = PORT | (1 << DS);
		} else {
			PORT = PORT & ~(1 << DS);
		}
		PORT |= (1 << CLK);
		_delay_ms(1);
	}
	
	
	for (k = 0; k < 8; k++) {
		PORT &=~ (1 << CLK);
		_delay_ms(1);
		
		if(sat3 >> k & 1) {
			PORT = PORT | (1 << DS);
		} else {
			PORT = PORT & ~(1 << DS);
		}
		PORT |= (1<<CLK);
		_delay_ms(1);
	}

	
	for (k = 0; k < 8; k++) {
		PORT &=~ (1 << CLK);
		_delay_ms(1);
		
		if (sat2 >> k & 1) {
			PORT = PORT | (1 << DS);
		} else {
			PORT = PORT & ~(1 << DS);
		}
		PORT |= (1<<CLK);
		_delay_ms(1);
	}
	
	
	for (k = 0; k < 8; k++) {
		PORT &=~ (1 << CLK);
		_delay_ms(1);
		
		if (sat1 >> k & 1) {
			PORT = PORT | (1 << DS);
		} else {
			PORT = PORT & ~(1 << DS);
		}
		PORT |= (1 << CLK);
		_delay_ms(1);
	}
	
	
	PORT |= (1<<E);
	PORT &= ~(1<<E);	
}

static void sat_text_anim()
{
	_delay_ms(1000);
	DDR |= (1<<CLK) | (1<<DS) | (1<<E); // output
	soft_spi(sat1, sat2, sat3, 0b00000000);
	sat_left_text_anim();
	_delay_ms(1000);
	sat_right_text_anim();	
}

static void sat_left_text_anim()
{
	
	for (int8_t k = 7; k >= 0; k--) {
		sat1 = sat1 | (1 << k);
		soft_spi(sat1, sat2, sat3, 0b00000000);
		_delay_ms(1000);
	}
}

static void sat_right_text_anim()
{
	
	for (int8_t k = 7; k >= 0; k--) {
		sat2 = sat2 | (1 << k);
		soft_spi(sat1, sat2, sat3, 0b00000000);
		_delay_ms(1000);
	}
}

static uint16_t get_level_by_channel(uint8_t channel)
{
	uint16_t maximum = 0;
	uint16_t result = 0;
	for (uint8_t i = 0; i <= 2; i++) {
		result = m4d_adc_get_level(channel);
		if (result > maximum) {
			maximum = result;
		}
	}
	
	return maximum;
}

void left_vu_timer(int8_t numDiodLeft)
{
	if (numDiodLeft >= prevDiodLeft) {
		
		if (peaks_mode == 3) {
			PORTD = 0;
			PORTD |= (1 << numDiodLeft);
		} else {
			for (int8_t i = prevDiodLeft; i <= numDiodLeft; i++) {
				PORTD |= (1 << i);
				PORTD |= (1 << numDiodLeft);
			}
		}

		prevDiodLeft = numDiodLeft;
		countLeft = 0;
	} else if (numDiodLeft <= prevDiodLeft) {
		
		if (countLeft < 65535) {
			countLeft++;
		}
		
		if (countLeft >= peaks_time) {
			if (peaks_mode == 3) {
				PORTD = 0;
				if (prevDiodLeft != 0) {
					PORTD |= (1 << (prevDiodLeft-1));
				}
			} else {
				PORTD &= ~(1 << (prevDiodLeft));
			}
			
			prevDiodLeft = prevDiodLeft - 1;
			if (prevDiodLeft < 0) {
				prevDiodLeft = 0;
			}
			countLeft = 0;
		}
	}
}

void reset_vu_array()
{
	for (int8_t i = 0; i < 20; i++) {
		right_vu_array[i] = 0;
	}
}

void right_vu_timer(int8_t numDiodRight)
{
	numDiodRight--;
	if (numDiodRight >= prevDiodRight) {
		if (peaks_mode == 3) {
			 reset_vu_array();
			right_vu_array[numDiodRight] = 1;	
		} else {
			for (int8_t i = prevDiodRight; i <= numDiodRight; i++) {
				VPORT |= (1 << (i + 1));
				VPORT |= (1 << (numDiodRight + 1));
			}
		}
		
		for (int8_t i = 0; i < 4; i++) { //1,2,3,4
			if (right_vu_array[i] == 1) {
				sat4 |= (1 << (i+1));
			} else {
				sat4 &= ~(1 << (i+1));
			}
		}
		
		for (int8_t i = 4; i < 9; i++) {//5,6,7,8
			if (right_vu_array[i] == 1) {
				sat3 |= (1 << (i-4));
			} else {
				sat3 &= ~(1 << (i-4));
			}
		}

		prevDiodRight = numDiodRight;
		soft_spi(sat1, sat2, sat3, sat4);
		countRight = 0;
	} else if (numDiodRight <= prevDiodRight) {
		
		if (countRight < 65535) {
			countRight++;
		}
		
		if (countRight >= peaks_time) {
			if (peaks_mode == 3) {
// 				VPORT = 0;
// 				if (prevDiodRight != 0) {
// 					VPORT |= (1 << (prevDiodRight - 1));
// 				}
				reset_vu_array();
				if (prevDiodRight != 0) {
					right_vu_array[prevDiodRight - 1] = 1;
					VPORT |= (1 << (prevDiodRight - 1));
				}					
			} else {
				VPORT &= ~(1 << (prevDiodRight));
			}
			
			prevDiodRight = prevDiodRight - 1;
			if (prevDiodRight < 0) {
				prevDiodRight = 0;
			}
			
			for (int8_t i = 0; i < 4; i++) {
				if (right_vu_array[i] == 1) {
					sat4 |= (1 << (i+1));
				} else {
					sat4 &= ~(1 << (i+1));
				}
			}
			
			for (int8_t i = 4; i < 9; i++) {
				if (right_vu_array[i] == 1) {
					sat3 |= (1 << (i-4));
				} else {
					sat3 &= ~(1 << (i-4));
				}
			}
			
			soft_spi(sat1, sat2, sat3, sat4);
			countRight = 0;
		}
	}
}

int main(void)
{
	DDRD = 0xFF;
	PORTD = 0x00;
	VPORT = 0x00;
	DDR |= (1<<CLK) | (1<<DS) | (1<<E); // output
	_delay_ms(50);
	//peaks_mode = EEPROM_read(PR_MODE_PEAKS_MODE);
	peaks_mode = 3;
	//peaks_time = EEPROM_read(PR_MODE_PEAKS_TIME) * 100;
	peaks_time = 110;
	set_peaks_mode(peaks_mode);


	soft_spi(sat1, sat2, sat3, 0x00);
	
	sat_text_anim();
	m4d_adc_init_8();
	_delay_ms(2000);
	
	i2c_init_as_slave();
	sei();
 	//peaks_mode = 1;
 	//peaks_time = 250;
uint8_t numDiodRight = 0;
uint8_t timer = 0;
uint16_t timer2 = 0;
    while (1) 
    {				
		//continue;
		execute_command_timer();
				
		if (peaks_mode == 0) {
			soft_spi(sat1, sat2, sat3, 0xff);
			PORTD = 0xff;
			continue;
		} else if (peaks_mode == 2) {
			soft_spi(sat1, sat2, sat3, 0x00);
			PORTD = 0x00;
			continue;
		}
	
		// Производим измерение
		uint16_t levelLeft = get_level_by_channel(LEFT_CHANNEL);
		uint16_t levelRight = get_level_by_channel(RIGHT_CHANNEL);
		int8_t numDiodLeft = getNumDiodByAdc(levelLeft);
		
		if (timer2 == 0) {
			timer++;
		}
		
		
		if (timer == 20) {
			numDiodRight++;
			if (numDiodRight > 9) {
				numDiodRight = 0;
				timer2++;
				sat3 |= (1 << 6);
			}
			timer = 0;
			
		}
		
		if (timer2 > 0) {
			timer2++;
			if (timer2 > 2000) {
				timer2 = 0;
				sat3 &= ~(1 << 6);
			}
		}

		
		left_vu_timer(numDiodLeft);
		right_vu_timer(numDiodRight);

    }
}