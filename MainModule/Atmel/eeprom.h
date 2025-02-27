/*
 * m4d_eeprom.h
 *
 * Created: 17.12.2020 1:28:58
 *  Author: m4d
 */ 


#ifndef M4D_EEPROM_H_
#define M4D_EEPROM_H_

#include "main.h"

void EEPROM_write(uint8_t uiAddress, unsigned char ucData);

unsigned char EEPROM_read(uint8_t uiAddress);

void EEPROM_write_word(uint16_t uiAddress, uint16_t ucData);

uint16_t EEPROM_read_word(uint16_t uiAddress);

uint32_t EEPROM_read_dword(uint16_t uiAddress);

void EEPROM_write_dword(uint16_t uiAddress, uint32_t ucData);


#endif /* M4D_EEPROM_H_ */