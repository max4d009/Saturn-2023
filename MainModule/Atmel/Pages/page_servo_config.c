/*
 * pageServoConfig.c
 *
 * ? Created: 29.11.2023 3:35:47
 *  Author: max-4d
 */ 
#include "page_servo_config.h"

struct ConfigParam menu_servo_params[MENU_SERVO_CONFIG_COUNT_PARAMS];

static uint8_t current_servo = SERVO_LEFT;
static uint8_t current_param = 0;
static uint8_t current_angle = 0;

PGM_P servo_names[4];

void init_page_servo_config()
{
	servo_names[SERVO_LEFT]   = PSTR("н.л.");
	servo_names[SERVO_REWIND] = PSTR("пер.");
	servo_names[SERVO_PLAY]   = PSTR("вос.");
	servo_names[SERVO_RIGHT]  = PSTR("нат.пр.");
	
	menu_servo_params[CONFIG_SERVO_MIN].name = PSTR("мин");
	menu_servo_params[CONFIG_SERVO_MIN].id = I2C_DATA_CONFIG_SERVO_MIN;
	menu_servo_params[CONFIG_SERVO_MIN].value = 0;
	
	menu_servo_params[CONFIG_SERVO_MAX].name = PSTR("макс");
	menu_servo_params[CONFIG_SERVO_MAX].id = I2C_DATA_CONFIG_SERVO_MAX;
	menu_servo_params[CONFIG_SERVO_MAX].value = 0;
	
	menu_servo_params[CONFIG_SERVO_PLAY].name = PSTR("восп");
	menu_servo_params[CONFIG_SERVO_PLAY].id = I2C_DATA_CONFIG_SERVO_PLAY;
	menu_servo_params[CONFIG_SERVO_PLAY].value = 0;
	
	menu_servo_params[CONFIG_SERVO_STOP].name = PSTR("стоп");
	menu_servo_params[CONFIG_SERVO_STOP].id = I2C_DATA_CONFIG_SERVO_STOP;
	menu_servo_params[CONFIG_SERVO_STOP].value = 0;
	
	menu_servo_params[CONFIG_SERVO_FORWARD].name = PSTR("впер");
	menu_servo_params[CONFIG_SERVO_FORWARD].id = I2C_DATA_CONFIG_SERVO_FORWARD;
	menu_servo_params[CONFIG_SERVO_FORWARD].value = 0;
	
	menu_servo_params[CONFIG_SERVO_REWIND].name = PSTR("наз");
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
	switch (current_servo) {
		case SERVO_PLAY:
			current_angle = i2c_data[I2C_DATA_SERVO_PLAY_CURRENT_ANGLE];
			break;
		case SERVO_REWIND:
			current_angle = i2c_data[I2C_DATA_SERVO_REWIND_CURRENT_ANGLE];
			break;
		case SERVO_LEFT:
			current_angle = i2c_data[I2C_DATA_SERVO_LEFT_CURRENT_ANGLE];
			break;
		case SERVO_RIGHT:
			current_angle = i2c_data[I2C_DATA_SERVO_RIGHT_CURRENT_ANGLE];
			break;
	}
	 
	char servo_name[strlen_P(servo_names[current_servo]) + 1];
	strcpy_P(servo_name, servo_names[current_servo]);
		
	oled_printf(0, -2, FONTID_6X8M, servo_name);
	
	for (uint8_t i2c_param = I2C_DATA_CONFIG_SERVO_MIN; i2c_param <= I2C_DATA_CONFIG_SERVO_SEARCH; i2c_param++) {
		uint8_t servo_parameter = i2c_param - I2C_DATA_CONFIG_SERVO_MIN;
		
		if (servo_parameter == current_param) {
			menu_servo_params[servo_parameter].value = current_angle;
		} else {
			menu_servo_params[servo_parameter].value = i2c_data[i2c_param];
		}
	}
	
	showConfigMenu(MENU_SERVO_CONFIG_COUNT_PARAMS, menu_servo_params, current_param, 2);
	
	oled_printf(0, 50, FONTID_6X8M, debug_string);
}

void page_servo_config_menu()
{
	current_param = 0;
	current_servo++;
	if (current_servo > NUM_SERVO - 1) {
		current_servo = 0;
	}
	i2c_send_config_current_servo(current_servo);
}

void page_servo_config_select()
{
	current_param++;
	if(current_param > MENU_SERVO_CONFIG_COUNT_PARAMS - 1) {
		current_param = 0;
	}
	i2c_send_position_servo(current_servo, menu_servo_params[current_param].value);
}

void page_servo_config_minus()
{
	if (current_angle - 1 > MIN) {
		i2c_send_position_servo(current_servo, current_angle - 1);
	}
}

void page_servo_config_plus()
{
	if (current_angle + 1 < MAX) {
		i2c_send_position_servo(current_servo, current_angle + 1);
	}
}

void page_servo_config_save()
{
	i2c_save_position_servo(current_servo, menu_servo_params[current_param].id, current_angle);
}


