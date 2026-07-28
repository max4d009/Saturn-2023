/*
 * pageFFT.c
 *
 * ? Created: 24.10.2023 18:21:57
 *  Author: m4d
 */ 
#include "page_fft.h"

static void set_freq_level(uint8_t freq_name, uint8_t freq_level);
static void change_gen_freq();
static void show_fft_background();
static void show_fft();
static void show_menu();

static uint8_t gen_freq_arr[10] = {1,2,7,20,60,100,120,140,160,200}; // f<=20 = f*50, f*100
static uint8_t f50 = 0, f100 = 0, f350 = 0, f1k = 0, f6k = 0, f10k = 0, f12k = 0, f14k = 0, f16k = 0, f20k = 0;
static uint8_t current_channel = CHANNEL_LEFT;
static uint8_t current_select = SELECT_PAGE_FFT_CHANNEL;
static uint8_t current_gen_freq = 0;
static uint8_t freq_gen_sended = 0;
static uint8_t need_wait_gen = 0;
static uint8_t wait_gen = 0;

void page_fft_render(uint8_t first_render)
{
	if (first_render == 1) {
	}

	show_menu();
	show_fft_background();
	
	if (is_rec_gen_mode() == 1) {
		change_gen_freq();
	}
	show_fft();
}

void page_fft_menu() 
{
	current.page = PAGE_OLED_TIMER;
}
	
void page_fft_select()
{
	current_select++;
	if (current_select >= SELECT_PAGE_FFT_NUM) {
		current_select = SELECT_PAGE_FFT_CHANNEL;
	}
}

void page_fft_minus()
{
	if (current_select == SELECT_PAGE_FFT_CHANNEL) {
		current_channel = CHANNEL_LEFT;
	} else if (current_select == SELECT_PAGE_FFT_BIAS) {
		if (current.bias > 0) {
			current.bias--;
			i2c_send_option_motherboard(I2C_MOTHERBOARD_SADP_OPTION, current.bias, 0);
		}
	} else if (current_select == SELECT_PAGE_FFT_UZ_EQ) {
		if (current.uz_eq > 0) {
			current.uz_eq--;
			i2c_send_option_motherboard(I2C_MOTHERBOARD_UZ_EQ_OPTION, current.uz_eq, 0);
		}
	}
}

void page_fft_plus()
{
	if (current_select == SELECT_PAGE_FFT_CHANNEL) {
		current_channel = CHANNEL_RIGHT;
	} else if (current_select == SELECT_PAGE_FFT_BIAS) {
		if (current.bias < 15){
			current.bias++;
			i2c_send_option_motherboard(I2C_MOTHERBOARD_SADP_OPTION, current.bias, 0);
		}
	} else if (current_select == SELECT_PAGE_FFT_UZ_EQ) {
		if (current.uz_eq < 3) {
			current.uz_eq++;
			i2c_send_option_motherboard(I2C_MOTHERBOARD_UZ_EQ_OPTION, current.uz_eq, 0);
		}
	}
}

void page_fft_save() {}

static void show_fft()
{	
	oled_draw_hline(7,   12,  f50, 1);   // 50 √ц
	oled_draw_hline(16,  24,  f100, 1);  // 100 √ц
	oled_draw_hline(28,  38,  f350, 1);  // 350 √ц
	oled_draw_hline(42,  47,  f1k, 1);   // 1 к√ц
	oled_draw_hline(51,  58,  f6k, 1);   // 6 к√ц
	oled_draw_hline(62,  71,  f10k, 1);  // 10 к√ц
	oled_draw_hline(75,  84,  f12k, 1);  // 12 к√ц
	oled_draw_hline(88,  97,  f14k, 1);  // 14 к√ц
	oled_draw_hline(101, 110, f16k, 1);  // 16 к√ц
	oled_draw_hline(113, 123, f20k, 1);  // 20 к√ц
}

void show_fft_background()
{
	char str_buf[2];  // ќдин буфер на 4 символа

	// ¬ерхн€€ строка (частоты) - все отдельные цифры и буквы
	sprintf_P(str_buf, PSTR("5"));   oled_printf(5, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("0"));   oled_printf(9, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("1"));   oled_printf(14, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("0"));   oled_printf(17, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("0"));   oled_printf(21, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("3"));   oled_printf(27, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("5"));   oled_printf(31, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("0"));   oled_printf(35, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("1"));   oled_printf(40, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("k"));   oled_printf(44, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("6"));   oled_printf(50, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("k"));   oled_printf(55, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("1"));   oled_printf(60, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("0"));   oled_printf(63, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("k"));   oled_printf(68, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("1"));   oled_printf(73, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("2"));   oled_printf(76, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("k"));   oled_printf(81, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("1"));   oled_printf(86, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("4"));   oled_printf(89, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("k"));   oled_printf(94, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("1"));   oled_printf(99, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("6"));   oled_printf(102, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("k"));   oled_printf(107, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("2"));   oled_printf(112, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("0"));   oled_printf(116, 50, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("k"));   oled_printf(121, 50, FONTID_6X8M, str_buf);

	// Ћева€ колонка
	sprintf_P(str_buf, PSTR("5"));   oled_printf(0, 15, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("0"));   oled_printf(0, 25, FONTID_6X8M, str_buf);
	sprintf_P(str_buf, PSTR("5"));   oled_printf(0, 35, FONTID_6X8M, str_buf);
	
	// === —етка (горизонтальные линии из точек) ===
	for (uint8_t i = 6; i <= 126; i += 10) {
		oled_draw_pixel(i, 19, 1);  // уровень 5
		oled_draw_pixel(i, 29, 1);  // уровень 0
		oled_draw_pixel(i, 39, 1);  // уровень 5
	}
	
	// === ќси координат ===
	oled_draw_vline(5, 10, 50, 1);     // вертикальна€ ось (Y от 10 до 50)
	oled_draw_hline(5, 126, 50, 1);    // горизонтальна€ ось (X от 5 до 126)
}

static void show_menu()
{
	char str_buf[15];
	
	if (current_channel == CHANNEL_LEFT) {
		oled_draw_menu_item(6, -2, FONTID_6X8M, current_select == SELECT_PAGE_FFT_CHANNEL ? 1:0, strcpy_P(str_buf, PSTR("k:лев")));
	} else {
		oled_draw_menu_item(6, -2, FONTID_6X8M, current_select == SELECT_PAGE_FFT_CHANNEL ? 1:0, strcpy_P(str_buf, PSTR("k:пр")));
	}
	
	oled_draw_menu_item(45, -2, FONTID_6X8M, current_select == SELECT_PAGE_FFT_BIAS ? 1:0, strcpy_P(str_buf, PSTR("подм:%02d")), current.bias);
	oled_draw_menu_item(96, -2, FONTID_6X8M, current_select == SELECT_PAGE_FFT_UZ_EQ ? 1:0, strcpy_P(str_buf, PSTR("эkв:%d")), current.uz_eq);
}

static void set_freq_level(uint8_t freq_name, uint8_t freq_level)
{
	uint8_t level = freq_level - 18 + V_BIAS;
	
	if (level > 49) {
		level = 49;
	}
	
	switch (freq_name) {
		case 1:
			f50 = level;
		break;
		case 2:
			f100 = level;
		break;
		case 7:
			f350 = level;
		break;
		case 20:
			f1k = level;
		break;
		case 60:
			f6k = level;
		break;
		case 100:
			f10k = level;
		break;
		case 120:
			f12k = level;
		break;
		case 140:
			f14k = level;
		break;
		case 160:
			f16k = level;
		break;
		case 200:
			f20k = level;
		break;
	}
}

static void change_gen_freq()
{
	if (freq_gen_sended == 0) {
		//if (current.servo_real_mode == REC_MODE_GEN) {
			i2c_send_freq_gen(gen_freq_arr[current_gen_freq]);
	//	}
		freq_gen_sended = 1;
		wait_gen = 0;
		return;
	}
	
	need_wait_gen = 1;
	if (current_gen_freq < 4) {
		need_wait_gen = 2;
	}
	
	if (freq_gen_sended == 1 && wait_gen != need_wait_gen) {
		wait_gen++;
		return;
	}

	if (audio_level.updated == 1) {
		if (current_channel == CHANNEL_LEFT) {
			set_freq_level(gen_freq_arr[current_gen_freq], audio_level.left);
		} else {
			set_freq_level(gen_freq_arr[current_gen_freq], audio_level.right);
		}
		
		current_gen_freq++;
		if (current_gen_freq >= 10) {
			current_gen_freq = 0;
		}
	
		freq_gen_sended = 0;
		audio_level.updated = 0;
		return;
	}
}