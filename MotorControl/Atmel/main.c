/*
 * speed.c
 * Основано на частометре http://www.leoniv.diod.club/ 
 * λ Created: 09.02.2021 0:45:37
 * Author : m4d
 */ 
#include "main.h"

unsigned long previousICR = 0;
uint16_t targetSpeed;
uint8_t disablePid;
double kp;
double ki;
double kd;
long minOut = 0;
long maxOut = 65535;
static long Freq;
long pwm;
uint8_t boost;
uint8_t st = 0;

uint8_t done = 0;

#define EPROM_ADDR_P 1
#define EPROM_ADDR_I 10
#define EPROM_ADDR_D 20
#define EPROM_ADDR_T 30
#define EPROM_ADDR_SPEED 80

static long Count_N = 0;   // количество входных периодов
char Count_M = 0;   // количество переполнений опорного счетчика
static unsigned int Tb, Te;		 // захваченные в начале и в конце значения
static double integral = 0, prevErr = 0;

long constrain(long amt, long low, long high)
{
	if(amt < low){
		return low;
	} else if (amt > high){
		return high;
	}
	return amt;
}

long computePID(long input)
{
	float dt = 0.003;
	long err = targetSpeed - input;
	integral = constrain_ard(integral + (double)err * dt * ki, minOut, maxOut);
	double D = (err - prevErr) / dt;
	prevErr = err;
	return constrain_ard(err * kp + integral + D * kd, minOut, maxOut);
}

ISR (TIMER1_CAPT_vect) 
{
	if (st == 0) {
		done = 0;
		Count_N = 0;
		Count_M = 0;
		Te = 0;
		Tb = ICR1;
		//Count_M = 0;
		if (!(Tb & 0x8000) && (TIFR1 & (1 << TOV1))) {
			Count_M--; // коррекция, если переполнение было до захвата
		}	    
		st = 1;
		//Count_N++;
	} else if (st == 2) {
		Te = ICR1;
		if (!(Te & 0x8000) &&  (TIFR1 & (1 << TOV1))) {
			Count_M++; // коррекция, если переполнение было до захвата
		}
			  
		long long m = (long)Count_M  * 65536L + Te - Tb; // интервал
		
		if(m != 0)
		  Freq = (long long)Count_N * F_CPU / m;
		else Freq = 0;

		Count_N = 0;
		Count_M = 0;
		Tb = 0;
		Te = 0;
		st = 0;

	} else {
		Count_N++;
		if (Count_N == 10) {
			st = 2;
		}
	}
}
  
ISR (TIMER1_OVF_vect) {
	if (st == 1 || st == 2) {
		Count_M++;
	}
}
  
void set_target_speed(uint16_t speed, uint8_t saveInEeprom)
{
	targetSpeed = speed;
}

void set_target_speed_from_eeprom()
{
// 	targetSpeed = EEPROM_read_word(EPROM_ADDR_SPEED);
// 	if(targetSpeed < 500 || targetSpeed > 1500){
// 		targetSpeed = 600;
// 		EEPROM_write_word(EPROM_ADDR_SPEED, 600);
// 	}
	targetSpeed = 1138;
	
}

int get_target_speed()
{
	return targetSpeed;
}

float get_real_speed()
{
	return Freq;
}


void set_brake_pid()
{
	maxOut = 22000;
	kp = 120;
	ki = 10;
	kd = 0.1;
}

void set_slow_pid()
{
	maxOut = 65535;
	kp = 10;
	ki = 0.5;
	kd = 0.05;
}

void set_fast_pid()
{
	maxOut = 26000;

	kp = 40;
	ki = 7;
	kd = 0.8;
}

void set_p(uint8_t p)
{
	EEPROM_write_word(EPROM_ADDR_P, p);
	kp = p;
}

void set_i(uint8_t i)
{
	EEPROM_write_word(EPROM_ADDR_I, i);
	ki = i;
}

void set_d(uint8_t d)
{
	EEPROM_write_word(EPROM_ADDR_D, d);
	kd = d;
}

int main(void)
{
	USART_ini(8);
	set_fast_pid();
	set_target_speed_from_eeprom();
// 	kp = EEPROM_read_word(EPROM_ADDR_P);
// 	ki = EEPROM_read_word(EPROM_ADDR_I);
// 	kd = EEPROM_read_word(EPROM_ADDR_D);
	
	
	//запрещение аналогового компаратора:
	ACSR = (1 << ACD);
	//разрешение таймера 1, CK/1:
	TCCR1B = (1 << CS10);
	//очистка отложенных прерываний:
	TIFR1 = (1 << TOV1) | (1 << ICF1);
	//разрешение прерываний по захвату и переполнению:
	TIMSK1 |= (1 << TOIE1) | (1 << ICIE1);
	
	// Режим Fast PWM
	DDRB &= ~(1 << PINB1);
	DDRB |= (1 << PINB2);
	PORTB  &= ~(1 << PINB1);
	PORTB  &= ~(1 << PINB2);

	TCCR1A |= (1 << COM1B1);
	TCCR1A |= (1 << WGM11) | (1 << WGM10);
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	OCR1A = 65535;
	OCR1B = 10000;
	
	sei();

    while (1) 
    {	
		pwm = computePID(Freq);
		OCR1B = pwm;
	}
}

