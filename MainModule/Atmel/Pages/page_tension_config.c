/*
 * page_tension_config.c
 *
 * Created: 23.01.2025 2:39:37
 *  Author: max4d
 */ 
#include "page_tension_config.h"

struct ConfigParam menu_pid_params[MENU_TENSION_CONFIG_COUNT_PARAMS];
static uint8_t current_pid = PID_REGULATOR_TENSION_PLAY;
static int16_t current_param_change = 0;
static uint8_t current_param = I2C_DATA_CONFIG_TENSION_P;

static void change_pid_koef(uint8_t up);

PGM_P tension_pid_type_names[3];

void init_page_tension_config()
{
	tension_pid_type_names[PID_REGULATOR_TENSION_PLAY]    = PSTR("натяж.воспр");
	tension_pid_type_names[PID_REGULATOR_TENSION_FORWARD] = PSTR("натяж.впер");
	tension_pid_type_names[PID_REGULATOR_TENSION_REWIND]  = PSTR("натяж.назад");
	
	menu_pid_params[CONFIG_TENSION_P].name = PSTR("P");
	menu_pid_params[CONFIG_TENSION_P].id = I2C_DATA_CONFIG_TENSION_P;
	menu_pid_params[CONFIG_TENSION_P].value = 0;
	
	menu_pid_params[CONFIG_TENSION_I].name = PSTR("I");
	menu_pid_params[CONFIG_TENSION_I].id = I2C_DATA_CONFIG_TENSION_I;
	menu_pid_params[CONFIG_TENSION_I].value = 0;
	
	menu_pid_params[CONFIG_TENSION_D].name = PSTR("D");
	menu_pid_params[CONFIG_TENSION_D].id = I2C_DATA_CONFIG_TENSION_D;
	menu_pid_params[CONFIG_TENSION_D].value = 0;
}

void page_tension_config_render(uint8_t first_render)
{
	char tension_pid_type_name[strlen_P(tension_pid_type_names[current_pid]) + 1];
	strcpy_P(tension_pid_type_name, tension_pid_type_names[current_pid]);
	
	char str_buf[10];
	oled_printf(0, -2, FONTID_6X8M, strcpy_P(str_buf, PSTR("коэф:%s")), tension_pid_type_name);
	
	for (uint8_t i2c_param = I2C_DATA_CONFIG_TENSION_P; i2c_param <= I2C_DATA_CONFIG_TENSION_D; i2c_param++) {
		uint8_t pid_parameter = i2c_param - I2C_DATA_CONFIG_TENSION_P;
		
		menu_pid_params[pid_parameter].value = i2c_data[i2c_param];
		
		// Для текущей настройки прибавляем измененное значение
		if (current_param == pid_parameter) {
			if((menu_pid_params[pid_parameter].value + current_param_change) < 0) {
				menu_pid_params[pid_parameter].value = 0;
			} else if ((menu_pid_params[pid_parameter].value + current_param_change) > 255) {
				menu_pid_params[pid_parameter].value = 255;
			} else {
				menu_pid_params[pid_parameter].value = menu_pid_params[pid_parameter].value + current_param_change;
			}
		}
		
		showConfigMenu(MENU_TENSION_CONFIG_COUNT_PARAMS, menu_pid_params, current_param);
	}
}

void page_tension_config_menu()
{
	current_param = 0;
	current_param_change = 0;
	current_pid++;
	if (current_pid > NUM_PID_REGULATOR - 1) {
		current_pid = 0;
	}
	i2c_send_config_current_pid(current_pid);
}

void page_tension_config_select()
{
	current_param_change = 0;
	current_param++;
	if(current_param > MENU_TENSION_CONFIG_COUNT_PARAMS - 1) {
		current_param = 0;
	}
}

void page_tension_config_minus()
{
	change_pid_koef(0);
}

void page_tension_config_plus()
{
	change_pid_koef(1);
}

void page_tension_config_save()
{
	i2c_save_pid_koef(current_pid, menu_pid_params[current_param].id, menu_pid_params[current_param].value);
	current_param_change = 0;
}

static void change_pid_koef(uint8_t up)
{
	if (up == 1 && menu_pid_params[current_param].value < 255) {
		current_param_change++;
	}
	
	if (up == 0 && menu_pid_params[current_param].value > 0) {
		current_param_change--;
	}
	
	i2c_send_pid_koef(current_pid, menu_pid_params[current_param].id, menu_pid_params[current_param].value);
}