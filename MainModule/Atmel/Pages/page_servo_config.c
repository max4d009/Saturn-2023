/*
 * pageServoConfig.c
 *
 * Created: 29.11.2023 3:35:47
 *  Author: max-4d
 */ 
#include "page_servo_config.h"

static void program_mode_servo_move(uint8_t up);
struct ConfigParam menu_servo_params[MENU_SERVO_CONFIG_COUNT_PARAMS];

static uint8_t current_servo = SERVO_LEFT;
static uint8_t current_param = I2C_DATA_CONFIG_SERVO_MIN;
static int16_t current_param_change = 0;
//static uint8_t current_angles[4];
PGM_P servo_names[4];

void init_page_servo_config()
{
	servo_names[SERVO_LEFT]   = PSTR("натяж.л.");
	servo_names[SERVO_REWIND] = PSTR("перем.");
	servo_names[SERVO_PLAY]   = PSTR("воспр.");
	servo_names[SERVO_RIGHT]  = PSTR("натяж.пр.");
	
	menu_servo_params[CONFIG_SERVO_MIN].name = PSTR("мин");
	menu_servo_params[CONFIG_SERVO_MIN].id = I2C_DATA_CONFIG_SERVO_MIN;
	menu_servo_params[CONFIG_SERVO_MIN].value = 0;
	
	menu_servo_params[CONFIG_SERVO_MAX].name = PSTR("макс");
	menu_servo_params[CONFIG_SERVO_MAX].id = I2C_DATA_CONFIG_SERVO_MAX;
	menu_servo_params[CONFIG_SERVO_MAX].value = 0;
	
	menu_servo_params[CONFIG_SERVO_PLAY].name = PSTR("воспр");
	menu_servo_params[CONFIG_SERVO_PLAY].id = I2C_DATA_CONFIG_SERVO_PLAY;
	menu_servo_params[CONFIG_SERVO_PLAY].value = 0;
	
	menu_servo_params[CONFIG_SERVO_STOP].name = PSTR("стоп");
	menu_servo_params[CONFIG_SERVO_STOP].id = I2C_DATA_CONFIG_SERVO_STOP;
	menu_servo_params[CONFIG_SERVO_STOP].value = 0;
	
	menu_servo_params[CONFIG_SERVO_FORWARD].name = PSTR("впер");
	menu_servo_params[CONFIG_SERVO_FORWARD].id = I2C_DATA_CONFIG_SERVO_FORWARD;
	menu_servo_params[CONFIG_SERVO_FORWARD].value = 0;
	
	menu_servo_params[CONFIG_SERVO_REWIND].name = PSTR("назад");
	menu_servo_params[CONFIG_SERVO_REWIND].id = I2C_DATA_CONFIG_SERVO_REWIND;
	menu_servo_params[CONFIG_SERVO_REWIND].value = 0;
	
	menu_servo_params[CONFIG_SERVO_PAUSE].name = PSTR("пауза");
	menu_servo_params[CONFIG_SERVO_PAUSE].id = I2C_DATA_CONFIG_SERVO_PAUSE;
	menu_servo_params[CONFIG_SERVO_PAUSE].value = 0;
	
	menu_servo_params[CONFIG_SERVO_SEARCH].name = PSTR("поиск");
	menu_servo_params[CONFIG_SERVO_SEARCH].id = I2C_DATA_CONFIG_SERVO_SEARCH;
	menu_servo_params[CONFIG_SERVO_SEARCH].value = 0;
}

void page_servo_config_render(uint8_t first_render)
{
	char servo_name[strlen_P(servo_names[current_servo]) + 1];
	strcpy_P(servo_name, servo_names[current_servo]);
	
	char str_buf[15];
		
	oled_printf(0, -2, FONTID_6X8M, strcpy_P(str_buf, PSTR("серво:%s")), servo_name);

	for (uint8_t i2c_param = I2C_DATA_CONFIG_SERVO_MIN; i2c_param <= I2C_DATA_CONFIG_SERVO_SEARCH; i2c_param++) {
			
		uint8_t servo_parameter = i2c_param - I2C_DATA_CONFIG_SERVO_MIN;
			
		menu_servo_params[servo_parameter].value = i2c_data[i2c_param];
			
		if (menu_servo_params[servo_parameter].value < MIN) {
			menu_servo_params[servo_parameter].value = MIN;
		}
		if (menu_servo_params[servo_parameter].value > MAX) {
			menu_servo_params[servo_parameter].value = MAX;
		}
			
		// Для текущей настройки прибавляем измененное значение
		if (current_param == servo_parameter) {
			if((menu_servo_params[servo_parameter].value + current_param_change) < 0) {
				menu_servo_params[servo_parameter].value = 0;
			} else {
				menu_servo_params[servo_parameter].value = menu_servo_params[servo_parameter].value + current_param_change;
			}
		}
	}
	
	showConfigMenu(MENU_SERVO_CONFIG_COUNT_PARAMS, menu_servo_params, current_param);

	oled_printf(0, 50, FONTID_6X8M, debug_string);
}

void page_servo_config_menu()
{
	current_param = 0;
	current_param_change = 0;
	current_servo++;
	if (current_servo > NUM_SERVO - 1) {
		current_servo = 0;
	}
	i2c_send_config_current_servo(current_servo);
}

void page_servo_config_select()
{
	current_param_change = 0;
	current_param++;
	if(current_param > MENU_SERVO_CONFIG_COUNT_PARAMS - 1) {
		current_param = 0;
	}
	i2c_send_position_servo(current_servo, menu_servo_params[current_param].value);
}

void page_servo_config_minus()
{
	program_mode_servo_move(0);
}

void page_servo_config_plus()
{
	program_mode_servo_move(1);
}

void page_servo_config_save()
{
	i2c_save_position_servo(current_servo, menu_servo_params[current_param].id, menu_servo_params[current_param].value);
	current_param_change = 0;
	_delay_ms(100); // Подождать инициализацию серво модуля
	i2c_send_position_servo(current_servo, menu_servo_params[current_param].value);
}

static void program_mode_servo_move(uint8_t up)
{
	if (up == 1 && menu_servo_params[current_param].value < MAX) {
		current_param_change++;	
	} 
	
	if (up == 0 && menu_servo_params[current_param].value > MIN) {
		current_param_change--;
	}
	
	i2c_send_position_servo(current_servo, menu_servo_params[current_param].value);
}


