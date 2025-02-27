/*
 * m4d_eeprom.c
 *
 * Created: 17.12.2020 1:29:13
 *  Author: m4d
 */ 

#include "m4d_eeprom.h"

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE)) //ждем освобождения флага окончания последней операцией с памятью
	{}
	EEAR = uiAddress; //Устанавливаем адрес
	EEDR = ucData; //Пишем данные в регистр
	EECR |= (1<<EEMPE); //Разрешаем запись
	EECR |= (1<<EEPE); //Пишем байт в память
}

unsigned char EEPROM_read(unsigned int uiAddress)
{
	while(EECR & (1<<EEPE))
	{} //ждем освобождения флага окончания последней операцией с памятью
	EEAR = uiAddress; //Устанавливаем адрес
	EECR |= (1<<EERE); //Запускаем операцию считывания из памяти в регистр данных
	return EEDR; //Возвращаем результат
}

void EEPROM_write_word(unsigned int uiAddress, uint16_t ucData)
{
	EEPROM_write(uiAddress, (unsigned char) ucData);
	unsigned char dt = ucData>>8;
	EEPROM_write(uiAddress+1, dt);
}

uint16_t EEPROM_read_word(unsigned int uiAddress)
{
	uint16_t dt = EEPROM_read(uiAddress+1)*256;
	asm("nop");
	dt += EEPROM_read(uiAddress);
	return dt;
}
