/*
 * mainboard.cpp
 *
 * λ Created: 06.11.2021 2:09:48
 * Author : m4d
 */ 

#include "main.h"

struct RelayPins relay_pins[RELAY_PINS_DDRD_COUNT+RELAY_PINS_DDRB_COUNT];
struct OptoPins opto_pins[11];
static void pwm_eq_init();

uint8_t current_mode = STOP_MODE;
volatile uint32_t mute_delay = 0;
static void pwm_bbe_set(uint8_t level);

void relay_all_pins_to_state(int state)
{
	uint8_t i;
	if (state) {
		for (i = 0; i < RELAY_PINS_DDRD_COUNT; i++) {
			PORTD |= (1 << RELAY_PINS_DDRD[i]);
		}
		for (i = 0; i < RELAY_PINS_DDRB_COUNT; i++) {
			PORTB |= (1 << RELAY_PINS_DDRB[i]);
		}		
	} else {
		for (i = 0; i < RELAY_PINS_DDRD_COUNT; i++) {
			PORTD &= ~(1 << RELAY_PINS_DDRD[i]);
		}
		for (i = 0; i < RELAY_PINS_DDRB_COUNT; i++) {
			PORTB &= ~(1 << RELAY_PINS_DDRB[i]);
		}		
	}	
}

void relay_switch(uint8_t state, uint8_t relay_id)
{
	if (state) {
		relay_all_pins_to_state(0);
		*relay_pins[relay_id].port |= (1 << relay_pins[relay_id].pin);
		_delay_ms(15);
		*relay_pins[relay_id].port &= ~(1 <<relay_pins[relay_id].pin);
		_delay_ms(10);
	} else {
		relay_all_pins_to_state(1);
		*relay_pins[relay_id].port &= ~(1 << relay_pins[relay_id].pin);
		_delay_ms(15);
		*relay_pins[relay_id].port |= (1 << relay_pins[relay_id].pin);
		_delay_ms(10);
	}
}

void optocoupler_switch(uint8_t state, uint8_t optocoupler_id)
{
	if (state) {
		*opto_pins[optocoupler_id].port |= (1 << opto_pins[optocoupler_id].pin);
	} else {
		*opto_pins[optocoupler_id].port &= ~(1 << opto_pins[optocoupler_id].pin);
	}
}

void optocouplers_init()
{
	opto_pins[OPTOCOUPLER_EQ_ID].pin = OPTO_EQ_PIN;
	opto_pins[OPTOCOUPLER_EQ_ID].port = &PORTB;
	DDRB |= (1 << OPTO_EQ_PIN);

	opto_pins[OPTOCOUPLER_NR_ID].pin = OPTO_NR_PIN;
	opto_pins[OPTOCOUPLER_NR_ID].port = &PORTC;
	DDRC |= (1 << OPTO_NR_PIN);
	
	opto_pins[OPTOCOUPLER_GEN_ID].pin = OPTO_GEN_PIN;
	opto_pins[OPTOCOUPLER_GEN_ID].port = &PORTC;
	DDRC |= (1 << OPTO_GEN_PIN);
	
	opto_pins[OPTOCOUPLER_UZ1_ID].pin = OPTO_UZ1_PIN;
	opto_pins[OPTOCOUPLER_UZ1_ID].port = &PORTC;
	DDRC |= (1 << OPTO_UZ1_PIN);

	opto_pins[OPTOCOUPLER_UZ0_ID].pin = OPTO_UZ0_PIN;
	opto_pins[OPTOCOUPLER_UZ0_ID].port = &PORTC;
	DDRC |= (1 << OPTO_UZ0_PIN);
	
	opto_pins[OPTOCOUPLER_SADP0_ID].pin = OPTO_SADP0_PIN;
	opto_pins[OPTOCOUPLER_SADP0_ID].port = &PORTB;
	DDRB |= (1 << OPTO_SADP0_PIN);
	
	opto_pins[OPTOCOUPLER_SADP1_ID].pin = OPTO_SADP1_PIN;
	opto_pins[OPTOCOUPLER_SADP1_ID].port = &PORTB;
	DDRB |= (1 << OPTO_SADP1_PIN);
		
	opto_pins[OPTOCOUPLER_SADP2_ID].pin = OPTO_SADP2_PIN;
	opto_pins[OPTOCOUPLER_SADP2_ID].port = &PORTB;
	DDRB |= (1 << OPTO_SADP2_PIN);
			
	opto_pins[OPTOCOUPLER_SADP3_ID].pin = OPTO_SADP3_PIN;
	opto_pins[OPTOCOUPLER_SADP3_ID].port = &PORTB;
	DDRB |= (1 << OPTO_SADP3_PIN);	
}

void relays_init()
{
	relay_pins[RELAY_GEN_ID].pin = RELAY_GEN_PIN;
	relay_pins[RELAY_GEN_ID].port = &PORTD;
	DDRD |= (1 << RELAY_GEN_PIN);
	
	relay_pins[RELAY_FIX_REC_ID].pin = RELAY_FIX_REC_PIN;
	relay_pins[RELAY_FIX_REC_ID].port = &PORTD;
	DDRD |= (1 << RELAY_FIX_REC_PIN);
	
	relay_pins[RELAY_KONTR_ID].pin = RELAY_KONTR_PIN;
	relay_pins[RELAY_KONTR_ID].port = &PORTD;
	DDRD |= (1 << RELAY_KONTR_PIN);
	
	relay_pins[RELAY_EQ_ID].pin = RELAY_EQ_PIN;
	relay_pins[RELAY_EQ_ID].port = &PORTD;
	DDRD |= (1 << RELAY_EQ_PIN);
	
	relay_pins[RELAY_GSP_ID].pin = RELAY_GSP_PIN;
	relay_pins[RELAY_GSP_ID].port = &PORTD;
	DDRD |= (1 << RELAY_GSP_PIN);
	
	relay_pins[RELAY_LINE_ID].pin = RELAY_LINE_PIN;
	relay_pins[RELAY_LINE_ID].port = &PORTB;
	DDRB |= (1 << RELAY_LINE_PIN);

	relay_pins[RELAY_REC_ID].pin = RELAY_REC_PIN;
	relay_pins[RELAY_REC_ID].port = &PORTB;
	DDRB |= (1 << RELAY_REC_PIN);
}

volatile uint8_t pwm_level = 0;   // 0...255
static uint16_t pwm_acc = 0;
// Инициализация программного ШИМ
static void pwm_eq_init() 
{
	pwm_bbe_set(0);
	// Настройка таймера 0 в обычный режим (счет до 255)
	TCCR0A = 0;                     // Обычный режим, без аппаратного ШИМ
	TCCR0B = (1 << CS00);           // ПРЕДДЕЛИТЕЛЬ 1 → 31.25 кГц (исправлено!)
	TIMSK0 |= (1 << TOIE0);         // Разрешить прерывание по переполнению
    DDRB |= (1 << OPTO_EQ_PIN);     // PB5 как выход
}

// Установка скважности (0-255)
static void pwm_bbe_set(uint8_t level) 
{
	pwm_level = level;
	pwm_acc = 0;
}

ISR(TIMER0_OVF_vect)
{
	// level = 0 → оптопара выключена → на выходе 9В
	if (pwm_level == 0)
	{
		PORTB &= ~(1 << OPTO_EQ_PIN);  // Постоянно LOW (0В) на PD6
		return;
	}

	// level = 255 → оптопара включена → на выходе 0В
	if (pwm_level == 255)
	{
		PORTB |= (1 << OPTO_EQ_PIN);   // Постоянно HIGH (5В) на PD6
		return;
	}

	pwm_acc += pwm_level;

	if (pwm_acc >= 256)
	{
		pwm_acc -= 256;
		PORTB |= (1 << OPTO_EQ_PIN);   // HIGH → оптопара включена
	}
	else
	{
		PORTB &= ~(1 << OPTO_EQ_PIN);  // LOW → оптопара выключена
	}
}

void set_mode(uint8_t mode)
{
	if (mode != REC_MODE_PLAY && mode != REC_MODE) {
		relay_switch(LINE_STATE_OFF, RELAY_LINE_ID);
	}
	
	if (mode != REC_MODE && mode != REC_MODE_GEN && mode != REC_MODE_PLAY) {
		relay_switch(GSP_STATE_OFF, RELAY_GSP_ID);
		relay_switch(REC_STATE_OFF, RELAY_REC_ID);
	}
	
	if (mode != REC_MODE_GEN && mode != REC_MODE_PLAY) {
		relay_switch(GEN_STATE_OFF, RELAY_GEN_ID);
		optocoupler_switch(0, OPTOCOUPLER_GEN_ID);
	}

	switch (mode) {
		case REC_MODE:
			relay_switch(GSP_STATE_ON, RELAY_GSP_ID);
			relay_switch(REC_STATE_ON, RELAY_REC_ID);
			relay_switch(LINE_STATE_ON, RELAY_LINE_ID);
			current_mode = mode;
		break;	
		case REC_MODE_GEN:
			optocoupler_switch(1, OPTOCOUPLER_GEN_ID);
			relay_switch(GSP_STATE_ON, RELAY_GSP_ID);
			relay_switch(REC_STATE_ON, RELAY_REC_ID);
			relay_switch(GEN_STATE_ON, RELAY_GEN_ID);
			current_mode = mode;
		break;			
		case PLAY_MODE:
		case REC_MODE_PLAY:	
			mute_delay = 1;
			current_mode = mode;
			break;
		default:
			current_mode = mode;
		break;
	}
}

void set_option(char option_, char value_)
{
	switch (option_) {
		case NR_OPTION:
			optocoupler_switch(value_, OPTOCOUPLER_NR_ID);
		break;
		
		case EQ_OPTION:
			relay_switch(value_, RELAY_EQ_ID);
		break;

		case KONTR_OPTION:
			relay_switch(value_, RELAY_KONTR_ID);
		break;

		case GEN_OPTION:
		break;
		
		case SADP_OPTION:
			optocoupler_switch(value_ & (1<<3), OPTOCOUPLER_SADP0_ID);
			optocoupler_switch(value_ & (1<<2), OPTOCOUPLER_SADP1_ID);
			optocoupler_switch(value_ & (1<<0), OPTOCOUPLER_SADP2_ID);
			optocoupler_switch(value_ & (1<<1), OPTOCOUPLER_SADP3_ID);
		break;
		
		case UZ_EQ_OPTION:
			optocoupler_switch(value_ & (1<<0), OPTOCOUPLER_UZ0_ID);
			optocoupler_switch(value_ & (1<<1), OPTOCOUPLER_UZ1_ID);
		break;
		
		case FIX_LEVEL_OPTION:
			relay_switch(value_, RELAY_FIX_REC_ID);
		break;
		
		case MUTE_OPTION:
			relay_switch(!value_, RELAY_LINE_ID);
		break;
		
		case BBE_OPTION:
			if (value_ == 0) {
				pwm_bbe_set(0);
			} else if (value_ == 1) {
				pwm_bbe_set(32);
			} else if (value_ == 2) {
				pwm_bbe_set(65);
			} else if (value_ == 3) {
				pwm_bbe_set(250);
			}
		break;
	}
}

int main(void)
{
	_delay_ms(100);
	m4d_i2c_init_as_slave();

	sei();
	relays_init();
	
 	optocouplers_init();
	
	relay_all_pins_to_state(0);

	set_mode(STOP_MODE);
	set_option(GEN_OPTION, 0);
 	optocoupler_switch(0, OPTOCOUPLER_GEN_ID);
	
	DDRD |=(1 << TEST_PD0_PIN);
	PORTD |= (1 << TEST_PD0_PIN);

	relay_switch(LINE_STATE_ON, RELAY_LINE_ID);
	pwm_eq_init();	
	
	while (1) {			
		execute_command_timer();
		
		if (mute_delay > 0) {
			mute_delay++;
			if (mute_delay == 400000) {
				relay_switch(LINE_STATE_ON, RELAY_LINE_ID);
				mute_delay = 0;
			}
		}
	}
}