/*
 * page_menu.c
 *
 * Created: 17.08.2024
 * Author: max4d
 */

#include "page_menu.h"

static uint8_t current_select = 0;
static uint8_t current_page = 0;

static void show_main_menu(uint8_t x, uint8_t y);

static const char menu_text[][11] PROGMEM =
{
	"Серво",
	"Уровень",
	"Скорость",
	"ДУ",
	"Индикатор",
	"Натяжение",
	"Эквалайзер"
};

static const uint8_t menu_page[PAGE_MENU_ITEMS_COUNT] PROGMEM =
{
	PAGE_OLED_SERVO_CONFIG,
	PAGE_OLED_LEVEL,
	PAGE_OLED_SPEED,
	PAGE_OLED_IR_BINDING,
	PAGE_OLED_VU,
	PAGE_OLED_TENSION_CONFIG,
	PAGE_OLED_EQ
};

/*--------------------------------------------------------------------
 * Отрисовка
 *------------------------------------------------------------------*/

void page_menu_render(uint8_t first_render)
{
	(void)first_render;

	show_main_menu(2, 0);

	oled_printf(
		120,
		0,
		FONTID_6X8M,
		"%d",
		current_page
	);
}

static void show_main_menu(uint8_t x, uint8_t y)
{
	uint8_t yy = y;

	uint8_t start =
		current_page * PAGE_MENU_ITEMS_ONE_PAGE_COUNT;

	uint8_t end =
		start + PAGE_MENU_ITEMS_ONE_PAGE_COUNT;

	if (end > PAGE_MENU_ITEMS_COUNT) {
		end = PAGE_MENU_ITEMS_COUNT;
	}

	for (uint8_t i = start; i < end; i++) {

		char menu_item_name[16];

		strcpy_P(menu_item_name, menu_text[i]);

		oled_draw_menu_item(
			x,
			yy,
			FONTID_6X8M,
			(current_select == i),
			menu_item_name
		);

		yy += 10;
	}
}

/*--------------------------------------------------------------------
 * Управление
 *------------------------------------------------------------------*/

void page_menu_menu()
{
	current_select++;

	if (current_select >= PAGE_MENU_ITEMS_COUNT) {
		current_select = 0;
	}

	current_page =
		current_select / PAGE_MENU_ITEMS_ONE_PAGE_COUNT;
}

void page_menu_select()
{
	current.page =
		pgm_read_byte(&menu_page[current_select]);
}

void page_menu_minus()
{
}

void page_menu_plus()
{
}

void page_menu_save()
{
}