//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------

#include <stdio.h>
#include "disp1color.h"
#include "ssd1306.h"

uint8_t disp1color_buff[(DISP1COLOR_Width * DISP1COLOR_Height) / 8];

//==============================================================================
// ��������� �������������� 1-������ �������
//==============================================================================
void disp1color_Init(void)
{
	#if (DISP1COLOR_type == DISPTYPE_ssd1306)
	  // ������������� �������
	  SSD1306_Init(DISP1COLOR_Width, DISP1COLOR_Height);
	  // ������� �������
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
// ��������� ��������� ������� Test �������
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
// ��������� ������������� ������� �������
//==============================================================================
void disp1color_SetBrightness(uint8_t Value)
{
#if (DISP1COLOR_type == DISPTYPE_ssd1306)
  SSD1306_SetContrast(Value);
#endif
}
//==============================================================================


//==============================================================================
// ��������� ��������� ����� ����� ��������� FillValue
//==============================================================================
void disp1color_FillScreenbuff(uint8_t FillValue)
{
  memset(disp1color_buff, FillValue, sizeof(disp1color_buff));
}
//==============================================================================


//==============================================================================
// ��������� ��������� ��������� ����������� � ������������ � ������� ����� disp1color_buff
//==============================================================================
void disp1color_UpdateFromBuff(void)
{
#if (DISP1COLOR_type == DISPTYPE_ssd1306)
  SSD1306_DisplayFullUpdate(disp1color_buff, sizeof(disp1color_buff)); 
#endif
}
//==============================================================================


//==============================================================================
// ��������� ������� �� ������� ��������������� ������
//==============================================================================
void oled_printf(uint8_t X, uint8_t Y, uint8_t FontID, const char *args, ...)
{
  char StrBuff[60];
  
  va_list ap;
  va_start(ap, args);
  vsnprintf(StrBuff, sizeof(StrBuff), args, ap);
  va_end(ap);
  
  disp1color_DrawString(X, Y, FontID, (uint8_t *)StrBuff);
}
//==============================================================================

//==============================================================================
// ��������� ������� �� ������� ����� ����
//==============================================================================
void oled_draw_menu_item(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t selected, const char *args, ...)
{
	char StrBuff[16];
	
	va_list ap;
	va_start(ap, args);
	vsnprintf(StrBuff, sizeof(StrBuff), args, ap);
	va_end(ap);
	
	disp1color_DrawString(X, Y, FontID, (uint8_t *)StrBuff);
	
	if (selected == 1) {
		uint8_t str_len = strlen(StrBuff);
		oled_draw_line(X-2, Y+8, X+(str_len * 6)-1, Y+8);
		oled_draw_pixel(X-2, Y+7, 1);
		oled_draw_pixel(X+(str_len * 6)-1, Y+7, 1);
	}
}
//==============================================================================


//==============================================================================
// ��������� ������������� ��������� 1 ������� �������
//==============================================================================
void oled_draw_pixel(uint8_t X, uint8_t Y, uint8_t State)
{
  // ���������, ��������� �� ����� � ���� ��������� �������
  if ((X >= DISP1COLOR_Width) || (Y >= DISP1COLOR_Height))
    return;
  
  uint16_t ByteIdx = Y >> 3;
  uint8_t BitIdx = Y - (ByteIdx << 3); // ������ ������������ ������ ���� (0<=Y<=7)
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
// ��������� ������ ������ ����� � ������ ����� �������
//==============================================================================
void oled_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2) 
{
  const int16_t deltaX = abs(x2 - x1);
  const int16_t deltaY = abs(y2 - y1);
  const int16_t signX = x1 < x2 ? 1 : -1;
  const int16_t signY = y1 < y2 ? 1 : -1;

  int16_t error = deltaX - deltaY;

  oled_draw_pixel(x2, y2, 1);

  while (x1 != x2 || y1 != y2) 
  {
    oled_draw_pixel(x1, y1, 1);
    const int16_t error2 = error * 2;
 
    if (error2 > -deltaY) 
    {
      error -= deltaY;
      x1 += signX;
    }
    if (error2 < deltaX)
    {
      error += deltaX;
      y1 += signY;
    }
  }
}
//==============================================================================


//==============================================================================
// ��������� ������ ������������� � ������ ����� �������
//==============================================================================
void oled_draw_rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2) 
{
  oled_draw_line(x1, y1, x1, y2);
  oled_draw_line(x2, y1, x2, y2);
  oled_draw_line(x1, y1, x2, y1);
  oled_draw_line(x1, y2, x2, y2);
}
//==============================================================================


//==============================================================================
// ��������� ������ ���������� � ������ ����� �������. x0 � y0 - ���������� ������ ����������
//==============================================================================
void disp1color_DrawCircle(int16_t x0, int16_t y0, int16_t radius) 
{
  int x = 0;
  int y = radius;
  int delta = 1 - 2 * radius;
  int error = 0;

  while (y >= 0)
  {
    oled_draw_pixel(x0 + x, y0 + y, 1);
    oled_draw_pixel(x0 + x, y0 - y, 1);
    oled_draw_pixel(x0 - x, y0 + y, 1);
    oled_draw_pixel(x0 - x, y0 - y, 1);
    error = 2 * (delta + y) - 1;

    if (delta < 0 && error <= 0) 
    {
      ++x;
      delta += 2 * x + 1;
      continue;
    }
	
    error = 2 * (delta - x) - 1;
		
    if (delta > 0 && error > 0) 
    {
      --y;
      delta += 1 - 2 * y;
      continue;
    }
	
    ++x;
    delta += 2 * (x - y);
    --y;
  }
}
//==============================================================================


//==============================================================================
// ������� ������ ������� Char �� �������. ���������� ������ ����������� �������
//==============================================================================
uint8_t disp1color_DrawChar(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t Char)
{
  // ��������� �� ����������� ����������� ������� ������
  uint8_t *pCharTable = font_GetFontStruct(FontID, FontID == FONTID_6X8M ? Char : Char - 46);
  uint8_t CharWidth = font_GetCharWidth(pCharTable);    // ������ �������
  uint8_t CharHeight = font_GetCharHeight(pCharTable);  // ������ �������
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


//==============================================================================
// ������� ������ ������ �� ������ Str �� �������
//==============================================================================
void disp1color_DrawString(uint8_t X, uint8_t Y, uint8_t FontID, uint8_t *Str)
{
  uint8_t done = 0;             // ���� ��������� ������
  uint8_t Xstart = X;           // ���������� ���� ����� ���������� ������� ��� �������� �� ����� ������
  uint8_t StrHeight = 8;        // ������ �������� � �������� ��� �������� �� ��������� ������

  // ����� ������
  while (!done)
  {
    switch (*Str)
    {
    case '\0':  // ����� ������
      done = 1;
      break;
    case '\n':  // ������� �� ��������� ������
      Y += StrHeight;
      break;
    case '\r':  // ������� � ������ ������
      X = Xstart;
      break;
    default:    // ������������ ������
      X += disp1color_DrawChar(X, Y, FontID, *Str);
      StrHeight = font_GetCharHeight(font_GetFontStruct(FontID, *Str));
      break;
    }
    Str++;
  }
}
//==============================================================================

void oled_show_stop(uint8_t x, uint8_t y)
{
	oled_draw_line(x, y,   x+4, y);
	oled_draw_line(x, y+1, x+4, y+1);
	oled_draw_line(x, y+2, x+4, y+2);
	oled_draw_line(x, y+3, x+4, y+3);
	oled_draw_line(x, y+4, x+4, y+4);
}

void oled_show_pause(uint8_t x, uint8_t y)
{
	oled_draw_line(x,   y, x, y+4);
	oled_draw_line(x+1, y, x+1, y+4);
	oled_draw_line(x+3, y, x+3, y+4);
	oled_draw_line(x+4, y, x+4, y+4);
}

void oled_show_play(uint8_t x, uint8_t y)
{
	oled_draw_line(x,   y,   x,   y+4);
	oled_draw_line(x+1, y,   x+1, y+4);
	oled_draw_line(x+2, y+1, x+2, y+3);
	oled_draw_line(x+3, y+2, x+3, y+2);
}

void oled_show_forward(uint8_t x, uint8_t y)
{
	oled_show_play(x, y);
	oled_show_play(x+4, y);
}

void oled_show_rewind(uint8_t x, uint8_t y)
{
	oled_draw_line(x,   y+2, x,   y+2);
	oled_draw_line(x+1, y+1, x+1, y+3);
	oled_draw_line(x+2, y,   x+2, y+4);
	oled_draw_line(x+3, y,   x+3, y+4);
	
	oled_draw_line(x+4, y+2, x+4, y+2);
	oled_draw_line(x+5, y+1, x+5, y+3);
	oled_draw_line(x+6, y,   x+6, y+4);
	oled_draw_line(x+7, y,   x+7, y+4);
}

void show_level_bar(uint8_t width, uint8_t x, uint8_t y, uint8_t level, uint8_t max, uint8_t invert)
{
	if (invert == 1) {
		for (uint8_t i = 0; i <= max; i++) {
			if (i >= level ) {
				oled_draw_line(x, i+y, x + width, i+y);
			}
		}
	} else {
		for (uint8_t i = level; i > 0; i--) {
			oled_draw_line(x, y + max-i, x + width, y + max - i);
		}
	}
}

void oled_show_repeat(uint8_t x, uint8_t y, uint8_t count)
{
	oled_printf(x+5, y+2, FONTID_6X8M, "%d", count);
	
	oled_draw_line(x+3, y+2, x+12, y+2);
	oled_draw_line(x+11, y+1, x+11, y+1);
	oled_draw_pixel(x+3, y+3, 1);
	oled_draw_pixel(x+3, y+4, 1);
	oled_draw_line(x+11, y+3, x+11, y+3);
	
	oled_draw_line(x+2, y+10, x+11, y+10);
	oled_draw_pixel(x+3, y+9, 1);
	oled_draw_pixel(x+3, y+11, 1);
	oled_draw_pixel(x+11, y+8, 1);
	oled_draw_pixel(x+11, y+9, 1);
}

void oled_show_conter_background()
{
	oled_draw_line(42, 35, 52, 35);
	oled_draw_line(42, 35, 42, 34);
	oled_draw_line(52, 35, 52, 34);
	oled_draw_line(47, 35, 47, 36);
	oled_printf(45, 38, FONTID_6X8M, "�");
	
	oled_draw_line(55, 35, 73, 35);
	oled_draw_line(55, 35, 55, 34);
	oled_draw_line(73, 35, 73, 34);
	oled_draw_line(64, 35, 64, 36);
	oled_printf(56, 38, FONTID_6X8M, "���");
	
	oled_draw_line(76, 35, 94, 35);
	oled_draw_line(76, 35, 76, 34);
	oled_draw_line(94, 35, 94, 34);
	oled_draw_line(85, 35, 85, 36);
	oled_printf(77, 38, FONTID_6X8M, "���");
	
	oled_draw_line(97, 35, 106, 35);
	oled_draw_line(97, 35, 97, 34);
	oled_draw_line(106, 35, 106, 34);
	oled_draw_line(102, 35, 102, 36);
	oled_printf(97, 38, FONTID_6X8M, "��");
}

void oled_show_eq(uint8_t eq)
{
	oled_printf(2, 16, FONTID_6X8M, "���");
	oled_draw_line(22, 18, 22, 22);
	oled_draw_line(26, 18, 26, 22);
	oled_draw_line(30, 18, 30, 22);
	oled_draw_line(25, 20, 27, 20);
	
	switch (eq) {
		case 0:
			oled_draw_line(21, 20, 23, 20);
			oled_draw_line(29, 20, 31, 20);
		break;
		case 1:
			oled_draw_line(21, 19, 23, 19);
			oled_draw_line(29, 19, 31, 19);
		break;
		case 2:
			oled_draw_line(21, 19, 23, 19);
			oled_draw_line(29, 18, 31, 18);
		break;
	}
}

void oled_show_info(char *str)
{
	disp1color_FillScreenbuff(0);
	oled_draw_rectangle(8, 8, 120, 56);
	oled_printf(14, 12, FONTID_6X8M, str);
	disp1color_UpdateFromBuff();
	_delay_ms(1000);
}