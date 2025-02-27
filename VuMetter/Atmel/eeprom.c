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
	while(EECR & (1<<EEPE)) //ждем освобождения флага окончания последней операцией с памятью
	{}
	EEAR = uiAddress; //Устанавливаем адрес
	EEDR = ucData; //Пишем данные в регистр
	EECR |= (1<<EEMPE); //Разрешаем запись
	EECR |= (1<<EEPE); //Пишем байт в память
}

unsigned char EEPROM_read(uint8_t uiAddress)
{
	while(EECR & (1<<EEPE))
	{} //ждем освобождения флага окончания последней операцией с памятью
	EEAR = uiAddress; //Устанавливаем адрес
	EECR |= (1<<EERE); //Запускаем операцию считывания из памяти в регистр данных
	return EEDR; //Возвращаем результат
}