/*
 * m4d_eeprom.h
 *
 * Created: 17.12.2020 1:28:58
 *  Author: m4d
 */ 


#ifndef M4D_EEPROM_H_
#define M4D_EEPROM_H_

#include "main.h"

void EEPROM_write(unsigned int uiAddress, unsigned char ucData); //пишем
unsigned char EEPROM_read(unsigned int uiAddress); //читаем
void EEPROM_write_word(unsigned int uiAddress, uint16_t ucData); //пишем 2х-байтовую величину
uint16_t EEPROM_read_word(unsigned int uiAddress); //читаем 2х-байтовую величину


#endif /* M4D_EEPROM_H_ */