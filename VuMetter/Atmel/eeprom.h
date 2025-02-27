/*
 * eeprom.h
 *
 * Created: 19.08.2024 2:29:31
 *  Author: max4d
 */ 
#ifndef EEPROM_H_
#define EEPROM_H_

#include "main.h"

#define PR_MODE_PEAKS_MODE 10
#define PR_MODE_PEAKS_TIME 20

void EEPROM_write(uint8_t uiAddress, unsigned char ucData);
unsigned char EEPROM_read(uint8_t uiAddress);

#endif /* EEPROM_H_ */