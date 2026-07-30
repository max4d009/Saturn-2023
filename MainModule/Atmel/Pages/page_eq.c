/*
 * page_eq.c
 *
 * Created: 20.06.2026 03:36:41
 *  Author: max4d
 */ 
#include "page_eq.h"

static uint8_t current_widget_id = 0;
struct SelectTextWidget text_widgets[TEXT_WIDGETS_COUNT];
struct SelectGaugeWidget gauge_widgets[GAUGE_WIDGETS_COUNT];
static void after_gauge_widget_update(struct SelectGaugeWidget* gauge_widget);
static void after_text_widget_update(struct SelectTextWidget* text_widget);
static void bh3864_set_bass_from_id(uint8_t id);
static void bh3864_set_treble_from_id(uint8_t id);
static void bh3864_set_tbass_by_id(uint8_t id);
static void bh3864_set_volume_from_id(uint8_t id);


void page_eq_render(uint8_t first_render)
{
	if (first_render == 1) {
		const bh3864_settings* settings = bh3864_get_settings();
		
		text_widgets[TEXT_WIDGET_EQ].items[TEXT_EQ_ROCK].name = PSTR("rock");
		text_widgets[TEXT_WIDGET_EQ].items[TEXT_EQ_POP].name = PSTR("pop");
		text_widgets[TEXT_WIDGET_EQ].items[TEXT_EQ_CLASSIC].name = PSTR("classic");
		text_widgets[TEXT_WIDGET_EQ].items_count = 3;
		text_widgets[TEXT_WIDGET_EQ].current_value = -1;
		text_widgets[TEXT_WIDGET_EQ].title = PSTR("eq");

		gauge_widgets[GAUGE_WIDGET_GEQ_LOW].items_count = 11;
		gauge_widgets[GAUGE_WIDGET_GEQ_LOW].current_value = settings->bass_id+1;
		gauge_widgets[GAUGE_WIDGET_GEQ_LOW].title = PSTR("bass");
		gauge_widgets[GAUGE_WIDGET_GEQ_LOW].zero_off = 1;
		gauge_widgets[GAUGE_WIDGET_GEQ_LOW].center_value = 5;
		gauge_widgets[GAUGE_WIDGET_GEQ_LOW].operation = (void (*)(uint8_t)) bh3864_set_bass_from_id;
		
		gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].items_count = 11;
		gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].current_value = settings->treble_id+1;
		gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].title = PSTR("tre");
		gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].zero_off = 1;
		gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].center_value = 5;
		gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].operation = (void (*)(uint8_t)) bh3864_set_treble_from_id;
		
		gauge_widgets[GAUGE_WIDGET_BBE].items_count = 3;
		gauge_widgets[GAUGE_WIDGET_BBE].current_value = current.bbe;
		gauge_widgets[GAUGE_WIDGET_BBE].title = PSTR("bbe");
		gauge_widgets[GAUGE_WIDGET_BBE].zero_off = 0;
		gauge_widgets[GAUGE_WIDGET_BBE].center_value = -1;
		gauge_widgets[GAUGE_WIDGET_BBE].operation = (void (*)(uint8_t)) set_bbe_option;
		
		gauge_widgets[GAUGE_WIDGET_TBASS].items_count = 4;
		gauge_widgets[GAUGE_WIDGET_TBASS].current_value = settings->tbass_id;
		gauge_widgets[GAUGE_WIDGET_TBASS].title = PSTR("tbass");
		gauge_widgets[GAUGE_WIDGET_TBASS].zero_off = 0;
		gauge_widgets[GAUGE_WIDGET_TBASS].center_value = -1;
		gauge_widgets[GAUGE_WIDGET_TBASS].operation = (void (*)(uint8_t)) bh3864_set_tbass_by_id;
		
		gauge_widgets[GAUGE_WIDGET_VOL].items_count = BH3864_VOL_DB_MIN;
		gauge_widgets[GAUGE_WIDGET_VOL].current_value = BH3864_INIT_VOL_DB + BH3864_VOL_DB_MIN;
		gauge_widgets[GAUGE_WIDGET_VOL].title = PSTR("vol");
		gauge_widgets[GAUGE_WIDGET_VOL].zero_off = 0;
		gauge_widgets[GAUGE_WIDGET_VOL].center_value = BH3864_INIT_VOL_DB + BH3864_VOL_DB_MIN - 1;
		gauge_widgets[GAUGE_WIDGET_VOL].operation = (void (*)(uint8_t)) bh3864_set_volume_from_id;
	}
		
	for (uint8_t i = 0; i < TEXT_WIDGETS_COUNT; i++) {
		show_text_item_widget(&text_widgets[i] ,2, i*9+6, 3, i == current_widget_id);
	}

	for (uint8_t i = 0; i < GAUGE_WIDGETS_COUNT; i++) {
		uint8_t widget_id = TEXT_WIDGETS_COUNT + i;
		show_gauge_item_widget(&gauge_widgets[i], 2, widget_id*9+6, current_widget_id == widget_id);
	}
	
	char str_buf[12];
	if (current.eq == 1) {
		strcpy_P(str_buf, PSTR("AIWA EQ ON"));
	} else {
		strcpy_P(str_buf, PSTR("AIWA EQ OFF"));
	}
	oled_printf(1, 0, FONTID_6X8M, str_buf);
}

void page_eq_menu()
{
	current.page = PAGE_OLED_TIMER;
}

void page_eq_select()
{
    current_widget_id++;
    if (current_widget_id >= ALL_WIDGETS_COUNT) {
	    current_widget_id = 0;
    }
}

void page_eq_minus()
{
	if (current_widget_id < TEXT_WIDGETS_COUNT) {

		struct SelectTextWidget* text_widget = &text_widgets[current_widget_id];

		if (text_widget->current_value > -1) {
			text_widget->current_value--;
		} else {
			text_widget->current_value =
			text_widget->items_count - 1;
		}
		after_text_widget_update(text_widget);

	} else {

		struct SelectGaugeWidget* gauge_widget = &gauge_widgets[current_widget_id - TEXT_WIDGETS_COUNT];

		if (gauge_widget->current_value > 0) {
			gauge_widget->current_value--;
			after_gauge_widget_update(gauge_widget);
		}
	}
}

void page_eq_plus()
{
	if (current_widget_id < TEXT_WIDGETS_COUNT) {

		struct SelectTextWidget* text_widget = &text_widgets[current_widget_id];

		if (text_widget->current_value < text_widget->items_count - 1) {
			text_widget->current_value++;
		} else {
			text_widget->current_value = -1;
		}
		after_text_widget_update(text_widget);
	} else {

		struct SelectGaugeWidget* gauge_widget = &gauge_widgets[current_widget_id - TEXT_WIDGETS_COUNT];

		if (gauge_widget->current_value < gauge_widget->items_count) {
			gauge_widget->current_value++;
			after_gauge_widget_update(gauge_widget);
		}
	}
}

static void after_text_widget_update(struct SelectTextWidget* text_widget)
{
	if (text_widget->current_value == -1) {
		gauge_widgets[GAUGE_WIDGET_GEQ_LOW].current_value = 5;
		gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].current_value = 5;
	} else if (text_widget->current_value == 0) {
		gauge_widgets[GAUGE_WIDGET_GEQ_LOW].current_value = 9;
		gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].current_value = 8;
	} else if (text_widget->current_value == 1) {
		gauge_widgets[GAUGE_WIDGET_GEQ_LOW].current_value = 3;
		gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].current_value = 3;
	} else if (text_widget->current_value == 2) {
		gauge_widgets[GAUGE_WIDGET_GEQ_LOW].current_value = 7;
		gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].current_value = 7;
	}
	gauge_widgets[GAUGE_WIDGET_GEQ_LOW].operation(gauge_widgets[GAUGE_WIDGET_GEQ_LOW].current_value);
	gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].operation(gauge_widgets[GAUGE_WIDGET_GEQ_HIGHT].current_value);
}

static void after_gauge_widget_update(struct SelectGaugeWidget* gauge_widget)
{
	uint8_t cur;
	if (gauge_widget->zero_off == 1) {
		if (gauge_widget->current_value == 0) {
			gauge_widget->current_value = 1;
			return;
		}
		cur = gauge_widget->current_value - 1;
	} else {
		cur = gauge_widget->current_value;
	}
	
	gauge_widget->operation(cur);
}

void page_eq_save()
{
}

static void bh3864_set_bass_from_id(uint8_t id)
{
	bh3864_set_bass_db((id-5)*2);
}

static void bh3864_set_treble_from_id(uint8_t id)
{
	bh3864_set_treble_db((id-5)*2);
}

static void bh3864_set_tbass_by_id(uint8_t id)
{
	bh3864_set_tbass_db(id*5);
}

static void bh3864_set_volume_from_id(uint8_t id)
{
	bh3864_set_volume_db(id - 81);
}