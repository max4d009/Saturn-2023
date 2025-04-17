/*
 * servo_core.c
 *
 * λ Created: 09.07.2024 18:44:35
 *  Author: max4d
 */ 

#include "servo_core.h"

static void update_servo_positions();
static void servo_timer_inc();
static void servo_timer_divide_x1();
static void servo_timer_divide_x2();
static void servo_timer_divide_x4();
static void servo_timer_divide_x6();
static void servo_timer_divide_x8();
static void servo_timer_divide_x10();
static void servo_timer_divide_x20();
static void servo_timer_divide_x60();
static void reels_timer();
static void emergency_shutdown_timer();
static volatile uint8_t servo_timer_takt = 0, servo_timer_via_one = 0, servo_timer_num = 0;
uint8_t servo_timer_on = 0;


void m4d_servo_init()
{
	// Начальные значения состояния кинематики
	kinematics_mode.current = STOP_MODE;
	kinematics_mode.previous = STOP_MODE;
	kinematics_mode.in_process = 0;
	kinematics_mode.slow_rew = 0;
	kinematics_mode.in_search = 0;
	kinematics_mode.search_step = SEARCH_STEP_0_STOP_SEARCH;
	kinematics_mode.tension = 0;
	kinematics_mode.tension_sensor_enable = 0;
	kinematics_mode.little_rewind_enable = 0;
	kinematics_mode.current_motor_speed = STOP_SPEED;
	kinematics_mode.need_motor_speed = STOP_SPEED;
	kinematics_mode.change_mode_counter = 0;
	kinematics_mode.motor_speed_play_correction = EEPROM_read(PR_MODE_MOTOR_SPEED);
	kinematics_mode.kinematics_speed = 1;
	kinematics_mode.repeat = 0;
	kinematics_mode.autostop_activated = 0;
	kinematics_mode.autostop_observer_enabled = 0;
	kinematics_mode.tension_calibrate_enable = 0;
	kinematics_mode.servo_left_saved_angle = 0;
	kinematics_mode.reel_size = 0;

	// Начальные значения делителей таймера
	servo_timer.div_x2  = 0;
	servo_timer.div_x4  = 0;
	servo_timer.div_x6  = 0;
	servo_timer.div_x8  = 0;
	servo_timer.div_x10 = 0;
	servo_timer.div_x20 = 0;
	servo_timer.div_x60 = 0;
	
	// Начальные значения скорости боковых узлов
	reels_speed.left_timer  = 0;
	reels_speed.right_timer = 0;
	reels_speed.left  = 0;
	reels_speed.right = 0;
	
	// Начальные параметры сервоприводов
	servo_list[SERVO_LEFT].pin   = SERVO_LEFT_PIN;
	servo_list[SERVO_REWIND].pin = REWIND_SERVO_PIN;
	servo_list[SERVO_PLAY].pin   = PLAY_SERVO_PIN;
	servo_list[SERVO_RIGHT].pin  = SERVO_RIGHT_PIN;

	for (uint8_t i = 0; i < NUM_SERVO; i++) {
		servo_list[i].stop_angle    = get_servo_eeprom_val(i, CONFIG_SERVO_STOP);
		servo_list[i].current_angle = get_servo_eeprom_val(i, CONFIG_SERVO_STOP);
		servo_list[i].need_angle    = get_servo_eeprom_val(i, CONFIG_SERVO_STOP);
		servo_list[i].max_angle     = get_servo_eeprom_val(i, CONFIG_SERVO_MAX);
		servo_list[i].min_angle     = get_servo_eeprom_val(i, CONFIG_SERVO_MIN);
		servo_list[i].play_angle    = get_servo_eeprom_val(i, CONFIG_SERVO_PLAY);
		servo_list[i].forward_angle = get_servo_eeprom_val(i, CONFIG_SERVO_FORWARD);
		servo_list[i].rewind_angle  = get_servo_eeprom_val(i, CONFIG_SERVO_REWIND);
		servo_list[i].pause_angle   = get_servo_eeprom_val(i, CONFIG_SERVO_PAUSE);
		servo_list[i].search_angle  = get_servo_eeprom_val(i, CONFIG_SERVO_SEARCH);
		servo_list[i].speed         = get_servo_eeprom_val(i, CONFIG_SERVO_SPEED);
	}
	servo_list[SERVO_PLAY].light_brake_angle   = 0;
	servo_list[SERVO_REWIND].light_brake_angle = 0;
	servo_list[SERVO_LEFT].light_brake_angle   = servo_list[SERVO_LEFT].stop_angle  + 40;
	servo_list[SERVO_RIGHT].light_brake_angle  = servo_list[SERVO_RIGHT].play_angle + 40;
	
	// Настройка шим
	OCR1A = 20000;
	TCCR1A = 0;
	TCCR1B |= (1 << WGM12);
	
	REEL_RIGHT_SENSOR_DDR &= ~(1 << REEL_RIGHT_SENSOR_PIN);
	REEL_RIGHT_SENSOR_PORT |= (1 << REEL_RIGHT_SENSOR_PIN);
	PCICR  |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT0);
	
	REEL_LEFT_SENSOR_DDR &= ~(1 << REEL_LEFT_SENSOR_PIN);
	REEL_LEFT_SENSOR_PORT |= (1 << REEL_LEFT_SENSOR_PIN);
	PCICR  |= (1 << PCIE2);
	PCMSK2 |= (1 << PCINT21);
	
	TCNT1 = 0;
	TCCR1B |= (1 << CS11);
	TIMSK1 |= (1 << OCIE1A);
	
	// Устанавливаем PB1,PB2,PB3,PB4 как выход
	SERVO_DDR |= (1 << SERVO_LEFT_PIN);
	SERVO_DDR |= (1 << REWIND_SERVO_PIN);
	SERVO_DDR |= (1 << PLAY_SERVO_PIN);
	SERVO_DDR |= (1 << SERVO_RIGHT_PIN);
	// Без подтягивающих резисторов
	SERVO_PORT &= ~(1 << SERVO_LEFT_PIN);
	SERVO_PORT &= ~(1 << REWIND_SERVO_PIN);
	SERVO_PORT &= ~(1 << PLAY_SERVO_PIN);
	SERVO_PORT &= ~(1 << SERVO_RIGHT_PIN);
	// Подать питание на сервоприводы
	SERVO_ENABLE_DDR  |= (1 << SERVO_ENABLE_PIN);
	SERVO_ENABLE_PORT |= (1 << SERVO_ENABLE_PIN);
	
	set_motor_speed(STOP_SPEED, 1);
}

// Самый быстрый таймер
static void servo_timer_divide_x1()
{	
	compute_all_adc_timer(); // Рассчёты АЦП
	search_program_timer();  // Поиск по паузам

	if (kinematics_mode.kinematics_speed == 2) {
		update_servo_positions(); // Обновление положения сервоприводов
	}
	
	emergency_shutdown_timer(); // Аварийное отключение при отключении питания
}

// В 2 раза медленней
static void servo_timer_divide_x2()
{
	execute_command_timer(); // Выполнить команды если какие то пришли по i2c
}

// В 3 раза медленней
static void servo_timer_divide_x3()
{
	if (kinematics_mode.kinematics_speed == 1) {
		update_servo_positions(); // Обновление положения сервоприводов
	}
}

// В 4 раза медленней
static void servo_timer_divide_x4()
{	
	change_mode_timer(kinematics_mode.current); // Переключение режимов кинематики
}

// В 6 раза медленней
static void servo_timer_divide_x6()
{
}

// В 8 раза медленней
static void servo_timer_divide_x8()
{
	if (kinematics_mode.kinematics_speed == 0) {
		update_servo_positions(); // Обновление положения сервоприводов
	}
}

// В 10 раза медленней
static void servo_timer_divide_x10()
{
}

// В 20 раза медленней
static void servo_timer_divide_x20()
{
	reels_timer(); // Рассчет скорости боковых узлов
}

// В 60 раза медленней
static void servo_timer_divide_x60()
{
}

// Тут пока вызов различных функций для которых нужна скорость боковых узлов
static void reels_timer()
{
	if (reels_speed.left > 0 && reels_speed.right > 0) {
		reels_speed.sum = reels_speed.left + reels_speed.right;
	} else {
		reels_speed.sum = 0;
	}
	
	reels_speed_timer(reels_speed.left, reels_speed.right, reels_speed.sum);
	
	reels_speed.left = 0;
	reels_speed.right = 0;
}

// Вызов функций-таймеров с различными делителями
static void servo_timer_inc()
{
	servo_timer_divide_x1();
	
	if (servo_timer.div_x2 == 1) {
		servo_timer_divide_x2();
		servo_timer.div_x2 = 0;
	}
	
	if (servo_timer.div_x3 == 2) {
		servo_timer_divide_x3();
		servo_timer.div_x3 = 0;
	}
	
	if (servo_timer.div_x4 == 3) {
		servo_timer_divide_x4();
		servo_timer.div_x4 = 0;
	}
	
	if (servo_timer.div_x6 == 5) {
		servo_timer_divide_x6();
		servo_timer.div_x6 = 0;
	}
	
	if (servo_timer.div_x8 == 7) {
		servo_timer_divide_x8();
		servo_timer.div_x8 = 0;
	}
	
	if (servo_timer.div_x10 == 9) {
		servo_timer_divide_x10();
		servo_timer.div_x10 = 0;
	}
	
	if (servo_timer.div_x20 == 19) {
		servo_timer_divide_x20();
		servo_timer.div_x20 = 0;
	}
	
	if (servo_timer.div_x60 == 59) {
		servo_timer_divide_x60();
		servo_timer.div_x60 = 0;
	}
	
	servo_timer.div_x2++; 
	servo_timer.div_x3++; 
	servo_timer.div_x4++; 
	servo_timer.div_x6++; 
	servo_timer.div_x8++; 
	servo_timer.div_x10++; 
	servo_timer.div_x20++; 
	servo_timer.div_x60++;
}

 // Обновление положения сервоприводов
static void update_servo_positions()
{
	uint16_t delta;
	for (uint8_t i = 0; i < NUM_SERVO; i++) {
		uint8_t speed = servo_list[i].speed;
		uint16_t current_angle = servo_list[i].current_angle;
		uint16_t need_angle = servo_list[i].need_angle;
		
		if (need_angle < servo_list[i].min_angle) {
			need_angle = servo_list[i].min_angle;
		}
		
		if (need_angle > servo_list[i].max_angle) {
			need_angle = servo_list[i].max_angle;
		}
		
		if (current_angle > need_angle) {
			delta = current_angle - need_angle;
			if(delta > 0){
				if((current_angle - speed) < need_angle) {
					servo_list[i].current_angle = need_angle;
				} else {
					servo_list[i].current_angle = current_angle - speed;
				}
			}
		} else if (current_angle < need_angle) {
			delta = need_angle - current_angle;
			if (delta > 0) {
				if ((current_angle + speed) > need_angle) {
					servo_list[i].current_angle = need_angle;
				} else {
					servo_list[i].current_angle = current_angle + speed;
				}
			}
		}
	}
}

// Аварийное отключение при отключении питания
static void emergency_shutdown_timer()
{
	if (!(PIND & (1 << PD4))) {
		servo_list[SERVO_PLAY].speed = 100;
		servo_list[SERVO_PLAY].need_angle = servo_list[SERVO_PLAY].stop_angle;
		update_servo_positions();
	}
}


ISR(PCINT0_vect)
{
	if (!(PINB & (1 << REEL_RIGHT_SENSOR_PIN))) {
		if (reels_speed.right_timer < 253) {
			reels_speed.right_timer++;
		}
		if (reels_speed.right < 253) {
			reels_speed.right++;
		}
		calc_search_overdo();
	}
}

ISR(PCINT2_vect)
{
	if (!(PIND & (1 << REEL_LEFT_SENSOR_PIN))) {
		if (reels_speed.left_timer < 253) {
			reels_speed.left_timer++;
		}
		if (reels_speed.left < 253) {
			reels_speed.left++;
		}
		calc_search_overdo();
	}
}

void servo_update_timer()
{
// 	if (servo_timer_on == 1) {
// 		servo_timer_inc();
// 		servo_timer_on = 0;
// 	}
}

// Здесь реализован программный шим для управления 4 сервоприводами
// Так же здесь вызываеся функция servo_timer_inc() таком образом, чтобы не портить шим сигнал.
// Т.е. по сути servo_timer_inc() основной таймер. В момент её вызова исполняюся весь нужный код, как если бы это был основной while в main
ISR(TIMER1_COMPA_vect)
{
	if (servo_timer_on == 1) {
		return;
	}
	
	if (servo_timer_via_one == 0) {
		SERVO_PORT |= (1 << servo_list[servo_timer_num].pin);
		OCR1A = servo_list[servo_timer_num].current_angle * 10;
	} else {
		SERVO_PORT &= ~(1 << servo_list[servo_timer_num].pin);
		OCR1A = CYCLE - servo_list[servo_timer_num].current_angle * 10;
	}
	
	if (servo_timer_via_one == 1) {
		servo_timer_via_one = 0;
		servo_timer_num++;
		servo_timer_inc();
		//servo_timer_on = 1;
	} else {
		servo_timer_via_one = 1;
	}
	
	servo_timer_takt++;
	if (servo_timer_takt == NUM_SERVO+NUM_SERVO) {
		servo_timer_takt = 0;
		servo_timer_num  = 0;
	}
}