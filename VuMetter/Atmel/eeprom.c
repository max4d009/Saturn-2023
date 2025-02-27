/*
 * eeprom.c
 *
 * Created: 19.08.2024 2:29:20
 *  Author: max4d
 */ 
#include "eeprom.h"

void EEPROM_write(uint8_t uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE)) //���� ������������ ����� ��������� ��������� ��������� � �������
	{}
	EEAR = uiAddress; //������������� �����
	EEDR = ucData; //����� ������ � �������
	EECR |= (1<<EEMPE); //��������� ������
	EECR |= (1<<EEPE); //����� ���� � ������
}

unsigned char EEPROM_read(uint8_t uiAddress)
{
	while(EECR & (1<<EEPE))
	{} //���� ������������ ����� ��������� ��������� ��������� � �������
	EEAR = uiAddress; //������������� �����
	EECR |= (1<<EERE); //��������� �������� ���������� �� ������ � ������� ������
	return EEDR; //���������� ���������
}