/*
 * λ search.c
 *
 * Created: 11.06.2024 1:29:06
 *  Author: max4d
 */ 

#include "search.h"

uint16_t overdo_sum = 0;
uint16_t count_stopped_overdo = 0;

static float expRunningAverageAdaptive(float newVal);

void search_program_timer()
{
	// Выходим если не в режиме поиска
	if (kinematics_mode.in_search == 0) {
		if (kinematics_mode.search_step != SEARCH_STEP_0_STOP_SEARCH) {
			kinematics_mode.search_step = SEARCH_STEP_0_STOP_SEARCH;
		}
		return;
	}

	if (kinematics_mode.search_step == SEARCH_STEP_0_STOP_SEARCH) {
		count_stopped_overdo = 0;
		overdo_sum = 0;
		kinematics_mode.search_step = SEARCH_STEP_1_FIND_PAUSE;
		return;
	}
	
	if (kinematics_mode.search_step == SEARCH_STEP_1_FIND_PAUSE) {
		return;
	}

	if (kinematics_mode.search_step == SEARCH_STEP_2_CALC_OVERDO) {
		count_stopped_overdo++;
		if (count_stopped_overdo > SEARCH_OVERDO_CALC_TIME) {
			kinematics_mode.search_step = SEARCH_STEP_3_OVERDO_CALCULATED;
		}
		return;
	}

	if (kinematics_mode.search_step == SEARCH_STEP_3_OVERDO_CALCULATED) {
		if (overdo_sum <= SEARCH_OVERDO_IGNORE) {
			kinematics_mode.in_search = 0;
			set_mode(PLAY_MODE, 1, 0);
			kinematics_mode.search_step = SEARCH_STEP_0_STOP_SEARCH;
			return;
		}
		
		if (kinematics_mode.previous == FORWARD_SEARCH_MODE) {
			set_mode(REWIND_MODE, 0, 0);
		} else if(kinematics_mode.previous == REWIND_SEARCH_MODE) {
			set_mode(FORWARD_MODE, 0, 0);
		}
		
		kinematics_mode.search_step = SEARCH_STEP_5_REWIND;
		return;
	}

	if (kinematics_mode.search_step == SEARCH_STEP_5_REWIND) {
		if (overdo_sum <= SEARCH_OVERDO_BEFORE_REACHING) {
			kinematics_mode.in_search = 0;
			set_mode(PLAY_MODE, 1, 0);
			kinematics_mode.search_step = SEARCH_STEP_0_STOP_SEARCH;
			return;
		}
	}
}

void search_pause_with_adc(uint16_t adc)
{
	static uint8_t silens_wait_time = 0;
	static uint16_t hight_level_time = 0;
	
	if (kinematics_mode.search_step != SEARCH_STEP_1_FIND_PAUSE) {
		hight_level_time = 0;
		silens_wait_time = 0;
		return;
	}
	
	adc = (uint16_t) expRunningAverageAdaptive(adc);
	
	// Ждём пока не появится стабильный уровень сигнала
	if (adc < SEARCH_ADC_SILENCE_LEVEL && hight_level_time <= SEARCH_LEVEL_WITHOUT_DROPS_TIME) {
		silens_wait_time = 0;
		hight_level_time = 0;
	} else if (adc > SEARCH_ADC_SILENCE_LEVEL && hight_level_time <= SEARCH_LEVEL_WITHOUT_DROPS_TIME) { // Проверяем, что уровень сигнала наблюдается некоторое время
		hight_level_time++;
	} else if (adc < SEARCH_ADC_SILENCE_LEVEL && hight_level_time > SEARCH_LEVEL_WITHOUT_DROPS_TIME && silens_wait_time < SEARCH_LEVEL_SILENCE_TIME) { // Считаем время в паузе
		if (silens_wait_time < SEARCH_LEVEL_SILENCE_TIME) {
			silens_wait_time++;
		}
	} else if (silens_wait_time >= SEARCH_LEVEL_SILENCE_TIME) { // Если обнаружена пауза
		set_mode(STOP_MODE, 0, 0);
		hight_level_time = 0;
		silens_wait_time = 0;
		kinematics_mode.search_step = SEARCH_STEP_2_CALC_OVERDO;
	}
}

void calc_search_overdo()
{
	if (kinematics_mode.search_step == SEARCH_STEP_2_CALC_OVERDO && overdo_sum < SEARCH_OVERDO_MAX) {
		overdo_sum++;
	}
	
	if (kinematics_mode.search_step == SEARCH_STEP_5_REWIND && overdo_sum > 0) {
		overdo_sum--;
	}
}

static float expRunningAverageAdaptive(float newVal)
{
	static float filVal = 0;
	float k;
	// резкость фильтра зависит от модуля разности значений
	if (abs(newVal - filVal) > 1.5) k = 0.9;
	else k = 0.03;
	
	filVal += (newVal - filVal) * k;
	return filVal;
}