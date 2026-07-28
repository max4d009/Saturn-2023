//------------------------------------------------------------------------------
// ? This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------

#include <stdio.h>
#include "disp1color.h"

uint8_t disp1color_buff[(DISP1COLOR_Width * DISP1COLOR_Height) / 8];
static void oled_draw_menu_gauge_segmented(uint8_t X, uint8_t Y, uint8_t height, uint8_t width, uint8_t current_value, uint8_t max_value, int8_t center_value);

//==============================================================================
// Процедура инициализирует 1-цветый дисплей
//==============================================================================
void disp1color_Init(void)
{
	#if (DISP1COLOR_type == DISPTYPE_ssd1306)
	  // Инициализация дисплея
	  SSD1306_Init(DISP1COLOR_Width, DISP1COLOR_Height);
	  // Очистка дисплея
	  disp1color_FillScreenbuff(0);
	  SSD1306_DisplayFullUpdate(disp1color_buff, sizeof(disp1color_buff));    
	#endif
}
//==============================================================================

void oled_sleep(void)
{
	#if (DISP1COLOR_type == DISPTYPE_ssd1306)
	
	 SSD1306_init_and_sleep();
	#endif
}

//==============================================================================
// Процедура управляет режимом Test дисплея
//==============================================================================
void disp1color_TestMode(uint8_t TestOn)
{
#if (DISP1COLOR_type == DISPTYPE_ssd1306)
  if (TestOn)
    SSD1306_AllPixOn();
  else
    SSD1306_AllPixRAM();
#endif
}
//==============================================================================


//==============================================================================
// Процедура устанавливает яркость дисплея
//==============================================================================
void disp1color_SetBrightness(uint8_t Value)
{
#if (DISP1COLOR_type == DISPTYPE_ssd1306)
  SSD1306_SetContrast(Value);
#endif
}
//==============================================================================


//==============================================================================
// Процедура заполняет буфер кадра значением FillValue
//==============================================================================
void disp1color_FillScreenbuff(uint8_t FillValue)
{
  memset(disp1color_buff, FillValue, sizeof(disp1color_buff));
}
//==============================================================================


//==============================================================================
// Процедура обновляет состояние индикаторов в соответствии с буфером кадра disp1color_buff
//==============================================================================
void disp1color_UpdateFromBuff(void)
{
#if (DISP1COLOR_type == DISPTYPE_ssd1306)
  SSD1306_DisplayFullUpdate(disp1color_buff, sizeof(disp1color_buff)); 
#endif
}
//==============================================================================


//==============================================================================
// Процедура выводит на дисплей форматированную строку
//==============================================================================
void oled_printf(uint8_t X, uint8_t Y, uint8_t FontID, const char *args, ...)
{
  char StrBuff[20];
  
  va_list ap;
  va_start(ap, args);
  vsnprintf(StrBuff, sizeof(StrBuff), args, ap);
  va_end(ap);
  
  oled_draw_string(X, Y, FontID, (uint8_t *)StrBuff);
}
//==============================================================================

//==============================================================================
// Процедура выводит на дисплей пункт меню
//==============================================================================
uint8_t oled_draw_menu_item(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t selected, const char *args, ...)
{
	char StrBuff[20];
	
	va_list ap;
	va_start(ap, args);
	vsnprintf(StrBuff, sizeof(StrBuff), args, ap);
	va_end(ap);
	
	uint8_t text_width = oled_draw_string(X, Y, FontID, (uint8_t *)StrBuff);

	if (selected == 1) {
		// === Рамка для текста ===
		oled_draw_hline(X - 2, X + text_width, Y + 8, 1);  // нижняя линия
		oled_draw_pixel(X - 2, Y + 7, 1);               // левый верхний угол
		oled_draw_pixel(X + text_width, Y + 7, 1);      // правый верхний угол
	}
	
	return text_width;
}
//==============================================================================

//==============================================================================
// Процедура выводит на дисплей пункт меню
//==============================================================================
uint8_t oled_draw_menu_item_rect(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t selected, const char *args, ...)
{
	char StrBuff[16];
	
	va_list ap;
	va_start(ap, args);
	vsnprintf(StrBuff, sizeof(StrBuff), args, ap);
	va_end(ap);
	
	uint8_t text_width = oled_draw_string(X, Y, FontID, (uint8_t *)StrBuff);

	if (selected == 1) {
		oled_draw_rectangle(X-2, Y, X+111 , Y+8);
	}
	
	return text_width;
}
//==============================================================================

//==============================================================================
// Процедура устанавливает состояние 1 пикселя дисплея
//==============================================================================
void oled_draw_pixel(uint8_t X, uint8_t Y, uint8_t State)
{
  // Проверяем, находится ли точка в поле отрисовки дисплея
  if ((X >= DISP1COLOR_Width) || (Y >= DISP1COLOR_Height))
    return;
  
  uint16_t ByteIdx = Y >> 3;
  uint8_t BitIdx = Y - (ByteIdx << 3); // Высота относительно строки байт (0<=Y<=7)
  ByteIdx *= DISP1COLOR_Width;  
  ByteIdx += X;
  
  if (State)
    disp1color_buff[ByteIdx] |= (1 << BitIdx);
  else
    disp1color_buff[ByteIdx] &= ~(1 << BitIdx);
}
//==============================================================================

void oled_draw_double_pixel(uint8_t X, uint8_t Y, uint8_t State)
{
	oled_draw_pixel(X, Y, State);
	oled_draw_pixel(X+1, Y, State);
	oled_draw_pixel(X, Y+1, State);
	oled_draw_pixel(X+1, Y+1, State);
}

void oled_draw_reel(uint8_t x, uint8_t y, uint8_t angle)
{
	if (angle == 0) {
		oled_draw_pixel(x-2, y, 1);
		oled_draw_pixel(x-1, y, 1);
		
		oled_draw_pixel(x, y, 1);
		
		oled_draw_pixel(x, y+1, 1);
		oled_draw_pixel(x+1, y+2, 1);
		
		oled_draw_pixel(x, y-1, 1);
		oled_draw_pixel(x+1, y-2, 1);
	} else if (angle == 1) {
		oled_draw_pixel(x, y-1, 1);
		oled_draw_pixel(x, y-2, 1);
		
		oled_draw_pixel(x, y, 1);
		
		oled_draw_pixel(x-2, y+1, 1);
		oled_draw_pixel(x-1, y, 1);
		
		oled_draw_pixel(x+1, y, 1);
		oled_draw_pixel(x+2, y+1, 1);
	} else if (angle == 2) {
		oled_draw_pixel(x, y, 1);
		oled_draw_pixel(x, y+1, 1);
		oled_draw_pixel(x-1, y+2, 1);
		
		oled_draw_pixel(x+1, y, 1);
		oled_draw_pixel(x+2, y, 1);
		
		oled_draw_pixel(x, y-1, 1);
		oled_draw_pixel(x-1, y-2, 1);		
	} else if (angle == 3) {
		oled_draw_pixel(x, y, 1);
		oled_draw_pixel(x-1, y, 1);
		oled_draw_pixel(x-2, y-1, 1);
		oled_draw_pixel(x+1, y, 1);
		oled_draw_pixel(x+2, y-1, 1);	
		oled_draw_pixel(x, y+1, 1);
		oled_draw_pixel(x, y+2, 1);
	}
}

//==============================================================================
// Процедура рисует прямую линию в буфере кадра дисплея
//==============================================================================
// void oled_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2) 
// {
//   const int16_t deltaX = abs(x2 - x1);
//   const int16_t deltaY = abs(y2 - y1);
//   const int16_t signX = x1 < x2 ? 1 : -1;
//   const int16_t signY = y1 < y2 ? 1 : -1;
// 
//   int16_t error = deltaX - deltaY;
// 
//   oled_draw_pixel(x2, y2, 1);
// 
//   while (x1 != x2 || y1 != y2) 
//   {
//     oled_draw_pixel(x1, y1, 1);
//     const int16_t error2 = error * 2;
//  
//     if (error2 > -deltaY) 
//     {
//       error -= deltaY;
//       x1 += signX;
//     }
//     if (error2 < deltaX)
//     {
//       error += deltaX;
//       y1 += signY;
//     }
//   }
// }
//==============================================================================

void oled_draw_vline(int16_t x, int16_t y1, int16_t y2, uint8_t state)
{
	if (y1 > y2) { int16_t tmp = y1; y1 = y2; y2 = tmp; } // меняем местами
	for (int16_t y = y1; y <= y2; y++) {
		oled_draw_pixel(x, y, state);
	}
}

void oled_draw_hline(int16_t x1, int16_t x2, int16_t y, uint8_t state)
{
	if (x1 > x2) { int16_t tmp = x1; x1 = x2; x2 = tmp; } // меняем местами
	for (int16_t x = x1; x <= x2; x++) {
		oled_draw_pixel(x, y, state);
	}
}

//==============================================================================
// Процедура рисует прямоугольник в буфере кадра дисплея
//==============================================================================
void oled_draw_rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2) 
{
    oled_draw_vline(x1, y1, y2, 1);  // левая сторона
    oled_draw_vline(x2, y1, y2, 1);  // правая сторона
    oled_draw_hline(x1, x2, y1, 1);  // верхняя сторона
    oled_draw_hline(x1, x2, y2, 1);  // нижняя сторона
}
//==============================================================================

//==============================================================================
// Процедура рисует закрашенный прямоугольник в буфере кадра дисплея
//==============================================================================
void oled_draw_filled_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t big)
{
	// Проверка и коррекция координат
	if (x1 > x2) {
		uint8_t temp = x1;
		x1 = x2;
		x2 = temp;
	}
	if (y1 > y2) {
		uint8_t temp = y1;
		y1 = y2;
		y2 = temp;
	}
	
	if (big == 1) {
		if (y1 > 0) y1 -= 1;  // вверх
		if (y2 < 63) y2 += 1; // вниз (63 - максимум для 64-пиксельного экрана)		
	}
	
	// Заливка прямоугольника построчно
	for (uint8_t y = y1; y <= y2; y++) {
		// Рисуем горизонтальную линию от x1 до x2
		oled_draw_hline(x1, x2, y, 1);
// 		for (uint8_t x = x1; x <= x2; x++) {
// 			oled_draw_pixel(x, y, 1);
// 		}
	}
}
//==============================================================================

//==============================================================================
// Процедура рисует окружность в буфере кадра дисплея. x0 и y0 - координаты центра окружности
//==============================================================================
// void disp1color_DrawCircle(int16_t x0, int16_t y0, int16_t radius) 
// {
//   int x = 0;
//   int y = radius;
//   int delta = 1 - 2 * radius;
//   int error = 0;
// 
//   while (y >= 0)
//   {
//     oled_draw_pixel(x0 + x, y0 + y, 1);
//     oled_draw_pixel(x0 + x, y0 - y, 1);
//     oled_draw_pixel(x0 - x, y0 + y, 1);
//     oled_draw_pixel(x0 - x, y0 - y, 1);
//     error = 2 * (delta + y) - 1;
// 
//     if (delta < 0 && error <= 0) 
//     {
//       ++x;
//       delta += 2 * x + 1;
//       continue;
//     }
// 	
//     error = 2 * (delta - x) - 1;
// 		
//     if (delta > 0 && error > 0) 
//     {
//       --y;
//       delta += 1 - 2 * y;
//       continue;
//     }
// 	
//     ++x;
//     delta += 2 * (x - y);
//     --y;
//   }
// }
//==============================================================================


//==============================================================================
// Функция вывода символа Char на дисплей. Возвращает ширину выведенного символа
//==============================================================================
uint8_t disp1color_DrawChar(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t Char)
{
  // Указатель на подтабличку конкретного символа шрифта
  uint8_t *pCharTable = font_GetFontStruct(FontID, FontID == FONTID_6X8M ? Char : Char - 46);
  uint8_t CharWidth = font_GetCharWidth(pCharTable);    // Ширина символа
  uint8_t CharHeight = font_GetCharHeight(pCharTable);  // Высота символа
  pCharTable += 2;
  
  if (FontID == FONTID_6X8M)
  {
    for (uint8_t row = 0; row < CharHeight; row++)
    {
      for (uint8_t col = 0; col < CharWidth; col++)
        oled_draw_pixel(X + col, Y + row, pCharTable[row] & (1 << (7 - col)));
    }
  }
  else
  {
    for (uint8_t row = 0; row < CharHeight; row++)
    {
      for (uint8_t col = 0; col < CharWidth; col++)
      {
        if (col < 8)
          oled_draw_pixel(X + col, Y + row, pCharTable[row * 2] & (1 << (7 - col)));
        else
          oled_draw_pixel(X + col, Y + row, pCharTable[(row * 2) + 1] & (1 << (15 - col)));
      }
    }
  }
  
  return CharWidth;
}
//==============================================================================

// Шрифт: font6x8[96][6] — символы от ' ' (32) до '~' (127)
// Каждый символ: 6 байт, каждый байт = 8 пикселей (по вертикали)
// В PROGMEM!
// extern const uint8_t font6x8[][6];
// 
// uint8_t fast_DrawChar(uint8_t x, uint8_t y, char c)
// {
// 	if (c < 32 || c > 127) c = '?'; // защита
// 	const uint8_t *glyph = font6x8[c - 32]; // указатель на символ
// 
// 	uint8_t page = y >> 3;       // номер страницы (каждая по 8 px)
// 	uint8_t shift = y & 0x07;    // смещение внутри страницы
// 	uint16_t baseIndex = page * DISP1COLOR_Width + x;
// 
// 	for (uint8_t col = 0; col < 6; col++) {
// 		uint8_t bits = pgm_read_byte(&glyph[col]);
// 
// 		if (shift == 0) {
// 			// Быстрый случай: ровно по страницам
// 			disp1color_buff[baseIndex + col] = bits;
// 		} else {
// 			// Общий случай: символ попадает на 2 страницы
// 			// Верхняя часть (сдвигаем вниз)
// 			disp1color_buff[baseIndex + col] |= bits << shift;
// 			// Нижняя часть (сдвигаем вверх и пишем в следующую страницу)
// 			if (page + 1 < (DISP1COLOR_Height >> 3)) {
// 				disp1color_buff[baseIndex + DISP1COLOR_Width + col] |= bits >> (8 - shift);
// 			}
// 		}
// 	}
// 
// 	return 6; // ширина символа
// }

//==============================================================================
// Функция вывода текста из строки Str на дисплей
//==============================================================================
uint8_t oled_draw_string(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t *Str)
{
	uint8_t done = 0;             // Флаг окончания вывода
	uint8_t Xstart = X;           // Запоминаем куда будем переводить каретку при переходе на новую строку
	uint8_t StrHeight = 8;        // Высота символов в пикселях для перехода на слежующую строку

	// Вывод строки
	while (!done)
	{
		switch (*Str)
		{
			case '\0':  // Конец строки
			done = 1;
			break;
			case '\n':  // Переход на следующую строку
			Y += StrHeight;
			break;
			case '\r':  // Переход в начало строки
			X = Xstart;
			break;
			default:    // Отображаемый символ
			X += disp1color_DrawChar(X, Y, FontID, *Str);
			StrHeight = font_GetCharHeight(font_GetFontStruct(FontID, *Str));
			break;
		}
		Str++;
	}
	
	return X - Xstart;
}
//==============================================================================

void oled_show_stop(uint8_t x, uint8_t y)
{
	oled_draw_hline(x, x + 4, y, 1);
	oled_draw_hline(x, x + 4, y + 1, 1);
	oled_draw_hline(x, x + 4, y + 2, 1);
	oled_draw_hline(x, x + 4, y + 3, 1);
	oled_draw_hline(x, x + 4, y + 4, 1);
}

void oled_show_pause(uint8_t x, uint8_t y)
{
	oled_draw_vline(x,     y, y + 4, 1);
	oled_draw_vline(x + 3, y, y + 4, 1);
}

void oled_show_play(uint8_t x, uint8_t y)
{
	oled_draw_vline(x,     y,     y + 4, 1);
	oled_draw_vline(x + 1, y,     y + 4, 1);
	oled_draw_vline(x + 2, y + 1, y + 3, 1);
	oled_draw_pixel(x + 3, y + 2, 1);
}

void oled_show_forward(uint8_t x, uint8_t y)
{
	oled_show_play(x, y);
	oled_show_play(x+4, y);
}

void oled_show_rewind(uint8_t x, uint8_t y)
{
	// === ЛЕВАЯ СТРЕЛКА ===
	oled_draw_pixel(x,     y + 2, 1);         // острие (центр)
	oled_draw_vline(x + 1, y + 1, y + 3, 1);  // средняя часть (3 пикселя)
	oled_draw_vline(x + 2, y,     y + 4, 1);  // левый край (5 пикселей)
	oled_draw_vline(x + 3, y,     y + 4, 1);  // правый край (5 пикселей)
	
	// === ПРАВАЯ СТРЕЛКА ===
	oled_draw_pixel(x + 4, y + 2, 1);         // острие (центр)
	oled_draw_vline(x + 5, y + 1, y + 3, 1);  // средняя часть (3 пикселя)
	oled_draw_vline(x + 6, y,     y + 4, 1);  // левый край (5 пикселей)
	oled_draw_vline(x + 7, y,     y + 4, 1);  // правый край (5 пикселей)
}

void show_level_bar(uint8_t width, uint8_t x, uint8_t y, uint8_t level, uint8_t max)
{
	if (level > max) level = max;

	level = max - level;

	for (uint8_t i = y+max; i > y+max-level; i--) {
		oled_draw_hline(x, x+width , i, 1);
	}
}

void oled_show_repeat(uint8_t x, uint8_t y, uint8_t count)
{
	oled_printf(x+5, y+2, FONTID_6X8M, "%d", count);
	
	// Верхняя стрелка
	oled_draw_hline(x+3, x+12, y+2, 1);
	oled_draw_pixel(x+11, y+1, 1);
	oled_draw_pixel(x+3, y+3, 1);
	oled_draw_pixel(x+3, y+4, 1);
	oled_draw_pixel(x+11, y+3, 1);
	
	// Нижняя стрелка
	oled_draw_hline(x+2, x+11, y+10, 1);
	oled_draw_pixel(x+3, y+9, 1);
	oled_draw_pixel(x+3, y+11, 1);
	oled_draw_pixel(x+11, y+8, 1);
	oled_draw_pixel(x+11, y+9, 1);
}

void oled_show_counter_background()
{
	char str_buf[4];
	// === Поле "ч" (часы) ===
	oled_draw_hline(42, 52, 35, 1);        // нижняя линия
	oled_draw_vline(42, 34, 35, 1);        // левая стенка (2 пикселя)
	oled_draw_vline(52, 34, 35, 1);        // правая стенка (2 пикселя)
	oled_draw_pixel(47, 36, 1);            // центральная точка внизу (для красоты)
	oled_printf(45, 38, FONTID_6X8M, strcpy_P(str_buf, PSTR("ч")));
	
	// === Поле "мин" (минуты) ===
	oled_draw_hline(55, 73, 35, 1);        // нижняя линия
	oled_draw_vline(55, 34, 35, 1);        // левая стенка
	oled_draw_vline(73, 34, 35, 1);        // правая стенка
	oled_draw_pixel(64, 36, 1);            // центральная точка
	oled_printf(56, 38, FONTID_6X8M, strcpy_P(str_buf, PSTR("мин")));
	
	// === Поле "сек" (секунды) ===
	oled_draw_hline(76, 94, 35, 1);        // нижняя линия
	oled_draw_vline(76, 34, 35, 1);        // левая стенка
	oled_draw_vline(94, 34, 35, 1);        // правая стенка
	oled_draw_pixel(85, 36, 1);            // центральная точка
	oled_printf(77, 38, FONTID_6X8M, strcpy_P(str_buf, PSTR("сек")));
	
	// === Поле "мс" (миллисекунды) ===
	oled_draw_hline(97, 106, 35, 1);       // нижняя линия
	oled_draw_vline(97, 34, 35, 1);        // левая стенка
	oled_draw_vline(106, 34, 35, 1);       // правая стенка
	oled_draw_pixel(102, 36, 1);           // центральная точка
	oled_printf(97, 38, FONTID_6X8M, strcpy_P(str_buf, PSTR("мс")));
}

void oled_show_eq(uint8_t eq)
{
	char str_buf[4];
	
	oled_printf(2, 16, FONTID_6X8M, strcpy_P(str_buf, PSTR("экв")));
	
	oled_draw_vline(22, 18, 22, 1); 
	oled_draw_vline(26, 18, 22, 1);
	oled_draw_vline(30, 18, 22, 1);
	
	oled_draw_hline(25, 27, 20, 1);
	
	// === Рисуем ползунок в зависимости от режима ===
	switch (eq) {
		case 0:
			oled_draw_hline(21, 23, 20, 1);
			oled_draw_hline(29, 31, 20, 1);
			break;
		case 1:
			oled_draw_hline(21, 23, 19, 1);
			oled_draw_hline(29, 31, 19, 1);
			break;
	}
}

void oled_show_info(char *str)
{
	disp1color_FillScreenbuff(0);
	oled_draw_rectangle(8, 8, 120, 56);
	oled_printf(14, 12, FONTID_6X8M, str);
	disp1color_UpdateFromBuff();
	_delay_ms(800);
}

void oled_show_result(uint16_t varValue)
{
	char str[10] = {'\0'};
	char value_char[5] = {'\0'};
	itoa(varValue, value_char, 10);
	
	char str_buf[3];
	strcat(str, strcpy_P(str_buf, PSTR("ok")));
	strcat(str, strcpy_P(str_buf, PSTR(":")));
	strcat(str, value_char);
	
	oled_show_info(str);
}

void oled_show_tension()
{
	// === Вертикальная шкала с тремя делениями ===
	oled_draw_vline(118, 26, 64, 1);        // основная вертикальная линия

	// Три горизонтальных деления
	oled_draw_hline(117, 118, 41, 1);       // верхнее деление
	oled_draw_hline(116, 118, 45, 1);       // среднее деление
	oled_draw_hline(117, 118, 49, 1);       // нижнее деление
	
	uint8_t current_tension = 0;
	if ((current.tension - 5) >= 5) {
		current_tension = current.tension - 5;
	}
	
	show_level_bar(8, 120, 5, current_tension, 64);
	char str_buf[5];
	oled_printf(94, 55, FONTID_6X8M, strcpy_P(str_buf, PSTR("нат:")));
}

void show_text_item_widget(struct SelectTextWidget *widget, uint8_t x, uint8_t y, uint8_t padding, uint8_t active_widget)
{
	char str_buf[16];
	strcpy_P(str_buf, widget->title);
	oled_draw_menu_item(x, y+padding+1, FONTID_6X8M, active_widget, str_buf);
	x = 31;

	for (int8_t i = 0; i < widget->items_count; i++) {
		strcpy_P(str_buf, widget->items[i].name);
		uint8_t item_len = oled_draw_menu_item(x, y+padding+1, FONTID_6X8M, widget->current_value == i ? 1 : 0, str_buf);
		x += padding+item_len;
	}
}

// X, Y координаты
// height высота шкалы
// width ОБЩАЯ ширина всей шкалы в пикселях (например, 100)
// current_value текущее знач
// max_value максимальное знач
static void oled_draw_menu_gauge_segmented(uint8_t X, uint8_t Y, uint8_t height, uint8_t width, uint8_t current_value, uint8_t max_value, int8_t center_value)
{
	if (max_value == 0) return;
	if (current_value > max_value) current_value = max_value;
	if (current_value == 0) return;
	
	uint8_t gap = (max_value > 50) ? 0 : 1;
	
	uint8_t total_width = width - gap * (max_value - 1);
	uint8_t segment_len = total_width / max_value;
	uint8_t remainder = total_width % max_value;
	
	if (segment_len < 1) segment_len = 1;
	
	// Накопленная ошибка для равномерного распределения
	uint8_t accumulated_error = 0;
	
	for (uint8_t i = 0; i < current_value; i++) {
		uint8_t current_segment_len = segment_len;
		
		// Равномерно распределяем остаток с помощью аккумуляции ошибки
		accumulated_error += remainder;
		if (accumulated_error >= max_value) {
			current_segment_len++;
			accumulated_error -= max_value;
		}
		
		oled_draw_filled_rectangle(X, Y, X + current_segment_len - 1, Y + height, i == center_value ? 1 : 0);
		X += current_segment_len + gap;
	}
}

void show_gauge_item_widget(struct SelectGaugeWidget *widget, uint8_t x, uint8_t y, uint8_t active_widget)
{
	char str_buf[16];
	strcpy_P(str_buf, widget->title);
	oled_draw_menu_item_rect(x, y+4, FONTID_6X8M, active_widget, str_buf);
	oled_draw_menu_gauge_segmented(31, y+6, 4, 81, widget->current_value, widget->items_count, widget->center_value);
}