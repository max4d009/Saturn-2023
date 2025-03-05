/*
 * search.h
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

#define SEARCH_ADC_SILENCE_LEVEL 40          // ������� �������, ������ �������� �������, ��� ������
#define SEARCH_LEVEL_WITHOUT_DROPS_TIME 40   // ����� � ������� �������� ������ �� ������ � ������, ����� ������� ��� ����������. ����� ������ ��������, ������ �����.
#define SEARCH_LEVEL_SILENCE_TIME 5  		 // ����� ������ ����� �������. ���� ��������� �������, ���������� ����� ����� �� �����������
#define SEARCH_OVERDO_MAX 10000			     // ������� �������� ������ �������� ����� ���� ��� ������� �����
#define SEARCH_OVERDO_IGNORE 400		     // ���� ������ �������� ��� ������� ��������� ����� ���������� �����, ����� �������� ���������������
#define SEARCH_OVERDO_BEFORE_REACHING 200	 // ������� �������� ����������� ��� ��������
#define SEARCH_OVERDO_CALC_TIME 400	         // ����� ���������� �� ������� ����������, ����� ��������� �����

void search_pause_with_adc(uint16_t adc);
void calc_search_overdo();
void search_program_timer();

#endif /* SEARCH_H_ */