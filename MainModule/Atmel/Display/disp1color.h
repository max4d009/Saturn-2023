//------------------------------------------------------------------------------
// λ This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#ifndef _DISP1COLOR_H
#define _DISP1COLOR_H

//#include "types.h"

// Список поддерживаемых дисплеев (контроллеров дисплеев)
#define DISPTYPE_ssd1306        0       // OLED-дисплей с контроллером ssd1306


// С каким типом дисплея будет работать модуль disp1color 
#define DISP1COLOR_type         DISPTYPE_ssd1306

// Размеры дисплея в пикселях
#define DISP1COLOR_Width        128
#define DISP1COLOR_Height       64


// Процедура инициализирует 1-цветый дисплей
void disp1color_Init(void);
void oled_sleep(void);
// Процедура управляет режимом Test дисплея
void disp1color_TestMode(uint8_t TestOn);
// Процедура устанавливает яркость дисплея
void disp1color_SetBrightness(uint8_t Value);
// Процедура заполняет буфер кадра значением FillValue
void disp1color_FillScreenbuff(uint8_t FillValue);
// Процедура обновляет состояние индикаторов в соответствии с буфером кадра
void disp1color_UpdateFromBuff(void);

// Процедура рисует прямую линию в буфере кадра дисплея
void oled_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
// Процедура рисует прямоугольник в буфере кадра дисплея
void oled_draw_rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
// Процедура рисует окружность в буфере кадра дисплея. x0 и y0 - координаты центра окружности
void disp1color_DrawCircle(int16_t x0, int16_t y0, int16_t radius);

// Функция вывода символа Char на дисплей. Возвращает ширину выведенного символа
uint8_t disp1color_DrawChar(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t Char);
// Функция вывода текста из строки Str на дисплей
void disp1color_DrawString(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t *Str);
// Процедура выводит на дисплей форматированную строку
void oled_printf(uint8_t X, uint8_t Y, uint8_t FontID, const char *args, ...);

void oled_draw_pixel(uint8_t X, uint8_t Y, uint8_t State);
void oled_draw_double_pixel(uint8_t X, uint8_t Y, uint8_t State);
void oled_draw_menu_item(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t selected, const char *args, ...);
void oled_draw_reel(uint8_t x, uint8_t y, uint8_t angle);
void oled_show_stop(uint8_t x, uint8_t y);
void oled_show_pause(uint8_t x, uint8_t y);
void oled_show_play(uint8_t x, uint8_t y);
void oled_show_forward(uint8_t x, uint8_t y);
void oled_show_rewind(uint8_t x, uint8_t y);
void show_level_bar(uint8_t width, uint8_t x, uint8_t y, uint8_t level, uint8_t max, uint8_t invert);
void oled_show_repeat(uint8_t x, uint8_t y, uint8_t count);
void oled_show_conter_background();
void oled_show_eq(uint8_t eq);
void oled_show_info(char *str);
#endif