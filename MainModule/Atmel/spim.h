//------------------------------------------------------------------------------
// ? This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _SPIM_H
#define _SPIM_H

#include "main.h"

typedef enum {
	SPI_MODE_0 = 0, // CPOL=0, CPHA=0
	SPI_MODE_1,     // CPOL=0, CPHA=1
	SPI_MODE_2,     // CPOL=1, CPHA=0
	SPI_MODE_3      // CPOL=1, CPHA=1
} SPIMode;

typedef enum {
	SPI_SPEED_DIV2 = 0,
	SPI_SPEED_DIV4,
	SPI_SPEED_DIV8,
	SPI_SPEED_DIV16,
	SPI_SPEED_DIV32,
	SPI_SPEED_DIV64,
	SPI_SPEED_DIV128
} SPISpeed;

// Процедура инициализации spi в режиме master
void spim_init(void);
// Процедура отправляет массив 16-битных слов
void SPI_send16b(uint16_t *pBuff, uint16_t Len);
// Процедура отправляет массив 8-битных слов
void SPI_send8b(uint8_t *pBuff, uint16_t Len);
// Процедура отправляет массив 16-битных слов
void SPI_recv16b(uint16_t *pBuff, uint16_t Len);
// Процедура отправляет массив 8-битных слов
void SPI_recv8b(uint8_t *pBuff, uint16_t Len);

#endif