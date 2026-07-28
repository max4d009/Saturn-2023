/*
 * main.c
 *
 * ? Author: max4d
 */ 
#include "main.h"
uint8_t tps = 50;

void resetParameters()
{
	current.on = 0;
	current.servo_real_mode = STOP_MODE;
	current.prev_mode = STOP_MODE;
	current.repeat = 0;
	current.nr = 0;
	current.eq = 0;
	current.kontr = 1;
	current.uz_eq = 0;
	current.fix_level = 0;
	current.bias = 3;
	current.page = PAGE_OLED_TIMER;
	current.debug = 0;
	current.reel_speed_left = 0;
	current.reel_speed_right = 0;
	current.motor_speed = 0;
	current.kinematics_speed = 1;
	current.tension = 0;
	current.mute = 1;
	
	reset_rec_page_inc();
	reset_rec_mode_inc();
	
	memset(debug_string, '.', DEBUG_STR_LEFT_MAX_LEN);
	timer_reset();
	_delay_ms(300);
	
	sei();
			
	i2c_send_option_motherboard(I2C_MOTHERBOARD_SADP_OPTION, current.bias, 1);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_UZ_EQ_OPTION, current.uz_eq, 1);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_EQ_OPTION, current.eq, 1);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_NR_OPTION, current.nr, 1);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_KONTR_OPTION, current.kontr, 1);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_FIX_LEVEL_OPTION, current.fix_level, 1);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_MUTE_OPTION, current.mute, 1);
	i2c_send_option_motherboard(I2C_MOTHERBOARD_BBE_OPTION, current.bbe, 1);
}

uint8_t is_rec_mode(uint8_t mode) 
{
	if (mode == REC_MODE || mode == REC_MODE_GEN || mode == REC_MODE_PLAY) {
		return 1;
	}
	
	return 0;
}

uint8_t is_rec_gen_mode()
{
	if (current.servo_real_mode == REC_MODE_GEN || (current.servo_real_mode == REC_MODE_PLAY && current.prev_mode == REC_MODE_GEN)) {
		return 1;
	}
	
	return 0;
}

void timer1_init(void)
{
	// Timer1, предделитель 64
	// Частота = 8 000 000 / 64 = 125000 Гц
	// Период = 8 мкс
	// Для 10 мс: 10000 / 8 = 1250 тиков
	// OCR1A = 1249
	
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);  // CTC, предделитель 64
	OCR1A = 1249;  // 1250 тиков × 8 мкс = 10000 мкс = 10 мс
	TIMSK1 = (1 << OCIE1A);  // Разрешаем прерывание
	sei();
}

static uint8_t cnt_20 = 0;
static uint8_t cnt_50 = 0;
static uint8_t cnt_100 = 0;
static uint8_t cnt_200 = 0;
static uint8_t cnt_300 = 0;
static volatile uint8_t timer_10ms = 0;
static volatile uint8_t flag_20ms = 0;
static volatile uint8_t flag_50ms = 0;
static volatile uint8_t flag_100ms = 0;
static volatile uint8_t flag_200ms = 0;
static volatile uint8_t flag_300ms = 0;
ISR(TIMER1_COMPA_vect)
{
    timer_10ms++;
    if (timer_10ms >= 100) timer_10ms = 0;
    
	cnt_20++;
    cnt_50++;
	cnt_100++;
    cnt_200++;
	cnt_300++;
	
	if (cnt_20 >= 2) {
		cnt_20 = 0;
		flag_20ms = 1;
	}
	
    if (cnt_50 >= 5) {
	    cnt_50 = 0;
	    flag_50ms = 1;
    }
	
	if (cnt_100 >= 10) {
		cnt_100 = 0;
		flag_100ms = 1;
	}
	   
    if (cnt_200 >= 20) {
	    cnt_200 = 0;
	    flag_200ms = 1;
    }
    if (cnt_300 >= 30) {
	    cnt_300 = 0;
	    flag_300ms = 1;
    } 
}

int main(void)
{
	DDRD |= (1 << IR_LED_PIN);
	DDRD |= (1 << STAND_BY_PIN);
	DDRD |= (1 << REC_LED_PIN);
	DDRD &= ~(1 << IR_PIN);
	PORTD |= (1 << IR_PIN);
	spim_init();
	disp1color_Init();
	_delay_ms(100);
	oled_sleep();
	
	manager_page_initPages();	
	adc_init_8();
	buttons_init();
	init_ir();
	timer1_init();
	adc_buttons_init();
	bh3864_init();
	current.vu_peaks_mode = 0;
	current.vu_peaks_time = 0;
	current.on = 0;
	set_sleep_mode(SLEEP_MODE_IDLE);
	while (1) 
	{
		if (get_ir_command_ready()) {
			reset_ir_command_ready();
			process_ir();
			continue;
		}
        if (IR_impulse_count != 0) continue;
				
		if (flag_20ms) {
			flag_20ms = 0;
			buttons_timer();
			send_from_query_timer();
		}
		
		if (current.on == 0) continue;

				
        if (flag_50ms) {
	        flag_50ms = 0;
			keyboard_adc_timer();
			manager_page_showPage();
			
        }
		
		if (flag_100ms) {
			flag_100ms = 0;
		    i2c_polling_timer(SERVO_ADDR, SLA_W_SERVO, SLA_R_SERVO);
			inc_counter();
		}
        
        if (flag_200ms) {
	        flag_200ms = 0;
			
        }
		
		if (flag_300ms) {
			flag_300ms = 0;
			off_timer();
			debug_led();
		}
		
		if (!flag_20ms && !flag_50ms && !flag_100ms && !flag_200ms  && !flag_300ms) {
			sleep_mode();
		}
	}
}

	
	