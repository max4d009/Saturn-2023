/*
 * λ search.h
 *
 * Created: 11.06.2024 1:29:20
 *  Author: max4d
 */ 
#ifndef SEARCH_H_
#define SEARCH_H_

#include "main.h"

#define SEARCH_STEP_0_STOP_SEARCH  0
#define SEARCH_STEP_1_FIND_PAUSE   1
#define SEARCH_STEP_2_CALC_OVERDO  2
#define SEARCH_STEP_3_OVERDO_CALCULATED  3
#define SEARCH_STEP_4_SET_MODE_BY_OVERDO 4
#define SEARCH_STEP_5_REWIND       5

#define SEARCH_ADC_SILENCE_LEVEL 40          // Уровень сигнала, меньше которого считаем, что тишина
#define SEARCH_LEVEL_WITHOUT_DROPS_TIME 40   // Время в течении которого сигнал не падает в тишину, чтобы считать его стабильным. Когда сигнал стабилен, начнем поиск.
#define SEARCH_LEVEL_SILENCE_TIME 5  		 // Вреня тишины между треками. Если поставить большое, котороткие паузы будут не учитаваться
#define SEARCH_OVERDO_MAX 10000			     // Сколько максимум учесть оборотов после того как найдена пауза
#define SEARCH_OVERDO_IGNORE 400		     // Если меньше оборотов чем указано произошло после нахождения паузы, сразу включить воспроизведение
#define SEARCH_OVERDO_BEFORE_REACHING 200	 // Сколько оборотов недокрутить при возврате
#define SEARCH_OVERDO_CALC_TIME 400	         // Время отведенное на подсчет перекрутки, после найденной паузы

void search_pause_with_adc(uint16_t adc);
void calc_search_overdo();
void search_program_timer();

#endif /* SEARCH_H_ */