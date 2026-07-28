/*
 * BH3864F.c
 */
#include "BH3864F.h"

static void bh3864_send_raw(uint32_t data);
static void bh3864_update(void);
static void bh3864_set_vol_combination(int8_t target_db, uint8_t channel);
static uint32_t bh3864_build_command(void);
static uint8_t reverse8(uint8_t v);
static uint8_t reverse_bits(uint8_t value, uint8_t bits);

// Volume A таблица
const int8_t bh3864_volA_dbs[32] PROGMEM  =  {
	0,   -2,  -4,  -6,  -8,  -10, -12, -14,
	-16, -18, -20, -22, -24, -26, -28, -30,
	-32, -34, -36, -38, -40, -42, -46, -50,
	-54, -58, -62, -66, -70, -74, -78, -82
};

// Volume B таблица
const int8_t bh3864_volB_dbs[4] PROGMEM = {0, -1, -2, -3};

// Bass и Treble таблица (D4-D7) (D8-D11)
const int8_t bh3864_geq_dbs[11] PROGMEM = {-10, -8, -6, -4, -2, 0, 2, 4, 6, 8, 10};
const uint8_t bh3864_geq_bits[11] PROGMEM = {0b1010, 0b0010, 0b1100, 0b0100, 0b1000, 0b0000, 0b1001, 0b0101, 0b1101, 0b0011, 0b1011};

// TBass таблица (D1-D3)
const uint8_t bh3864_tbass_dbs[5] PROGMEM = {0, 5, 10, 15, 20};
const uint8_t bh3864_tbass_bits[5] PROGMEM = {0b000, 0b100, 0b010, 0b110, 0b001};

static bh3864_settings bh3864f_settings = {
	.volA_ch1_id = 0,
	.volB_ch1_id = 0,
	.volA_ch2_id = 0,
	.volB_ch2_id = 0,
	.bass_id = 5, 
	.treble_id = 5,
	.tbass_id = 0,
	.gain_id = 0 
};

void bh3864_init(void)
{
    DDRD |= (1 << BH_LATCH);
    LATCH_LOW();
	
	bh3864_set_volume_db(BH3864_INIT_VOL_DB);
	bh3864_update();
}

const bh3864_settings* bh3864_get_settings(void)
{
	return &bh3864f_settings;
}

void bh3864_set_volume_db(int8_t db)
{
	bh3864_set_channel_volume_db(db, db);
}

void bh3864_set_channel_volume_db(int8_t ch1_db, int8_t ch2_db)
{
	bh3864_set_vol_combination(ch1_db, BH3864_LEFT_CHANNEL);
	bh3864_set_vol_combination(ch2_db, BH3864_RIGHT_CHANNEL);
	bh3864_update();
}

void bh3864_set_bass_db(int8_t db)
{
	for (uint8_t i = 0; i < 11; i++) {
		if ((int8_t)pgm_read_byte(&bh3864_geq_dbs[i]) == db) {
			bh3864f_settings.bass_id = i;
			bh3864_update();
			return;
		}
	}
}

void bh3864_set_treble_db(int8_t db)
{
	for (uint8_t i = 0; i < 11; i++) {
		if ((int8_t)pgm_read_byte(&bh3864_geq_dbs[i]) == db) {
			bh3864f_settings.treble_id = i;
			bh3864_update();
			return;
		}
	}
}

void bh3864_set_tbass_db(uint8_t db)
{
	for (uint8_t i = 0; i < 5; i++) {
		if (pgm_read_byte(&bh3864_tbass_dbs[i]) == db) {
			bh3864f_settings.tbass_id = i;
			bh3864_update();
			return;
		}
	}
}

static void bh3864_set_vol_combination(int8_t target_db, uint8_t channel)
{
	if (target_db <= -128) {
		if (channel == BH3864_LEFT_CHANNEL) {
			bh3864f_settings.volA_ch1_id = 31;
			bh3864f_settings.volB_ch1_id = 0;
		} else {
			bh3864f_settings.volA_ch2_id = 31;
			bh3864f_settings.volB_ch2_id = 0;
		}
		return;
	}
	
	// Ограничиваем диапазон
	if (target_db > 0) target_db = 0;
	if (target_db < -81) target_db = -81;
	
	uint8_t vol_b_max;
	for (uint8_t vol_a = 0; vol_a <= BH3864_VOL_A_MAX; vol_a++) {
		
		if (vol_a >= BH3864_VOL_B_LIMIT) {
			vol_b_max = BH3864_VOL_B_MAX_AFTER_LIMIT;
		} else {
			vol_b_max = BH3864_VOL_B_MAX_BEFORE_LIMIT;
		}
		
		for (uint8_t vol_b = 0; vol_b <= vol_b_max; vol_b++) {
			int8_t att = pgm_read_byte(&bh3864_volA_dbs[vol_a]) + pgm_read_byte(&bh3864_volB_dbs[vol_b]);
			if (att == target_db) {
				// Записываем найденные значения в соответствующий канал
				if (channel == BH3864_LEFT_CHANNEL) {
					bh3864f_settings.volA_ch1_id = vol_a;
					bh3864f_settings.volB_ch1_id = vol_b;
				} else {
					bh3864f_settings.volA_ch2_id = vol_a;
					bh3864f_settings.volB_ch2_id = vol_b;
				}
				return;
			}
		}
	}
}

static void bh3864_update(void)
{
	uint32_t cmd = bh3864_build_command();
	
	SSD1306_CS_HIGH();
	SPCR &= ~(1 << SPE);
	
	bh3864_send_raw(cmd);
	
	SPCR |= (1 << SPE);
	SSD1306_CS_LOW();
}

// Построение 28-битной команды
static uint32_t bh3864_build_command(void)
{
	uint32_t cmd = 0;
	
	// Chip select (D26, D27) = 1,1
	cmd |= (1UL << 27);
	cmd |= (1UL << 26);
	
	// Volume B CH1 (D24, D25)
	cmd |= (((uint32_t)reverse_bits(bh3864f_settings.volB_ch1_id, 2) & 0x03) << 24);
	// Volume A CH1 (D19-D23)
	cmd |= (((uint32_t)reverse_bits(bh3864f_settings.volA_ch1_id, 5) & 0x1F) << 19);
	// Volume B CH2 (D17, D18)
	cmd |= (((uint32_t)reverse_bits(bh3864f_settings.volB_ch2_id, 2) & 0x03) << 17);
	// Volume A CH2 (D12-D16)
	cmd |= (((uint32_t)reverse_bits(bh3864f_settings.volA_ch2_id, 5) & 0x1F) << 12);
	// Treble (D8-D11)
	cmd |= (((uint32_t)(pgm_read_byte(&bh3864_geq_bits[bh3864f_settings.bass_id]) & 0x0F)) << 8);
	// Bass (D4-D7)
	cmd |= (((uint32_t)(pgm_read_byte(&bh3864_geq_bits[bh3864f_settings.treble_id]) & 0x0F)) << 4);
	// Dynamic Bass (D1-D3)
	cmd |= (((uint32_t)(pgm_read_byte(&bh3864_tbass_bits[bh3864f_settings.tbass_id]) & 0x07)) << 1);
	// Gain (D0)
	cmd |= ((uint32_t)(bh3864f_settings.gain_id & 0x01));
	
	return cmd;
}

// Отправка 28 бит
static void bh3864_send_raw(uint32_t data)
{
	SCK_LOW();
	LATCH_LOW();
	DATA_LOW();
	
	_delay_us(5);
	// Передача 28 бит: от D0 до D27 (D27 последний)
	for (uint8_t i = 0; i < 28; i++) {
		SCK_LOW();
		_delay_us(5);
		
		// Устанавливаем DATA (используем 1UL для 32-битного сдвига)
		if (data & (1UL << i)) {
			DATA_HIGH();
		} else {
			DATA_LOW();
		}
	
		_delay_us(5);
		SCK_HIGH();
		_delay_us(5);
	}
	
	// Пауза перед LATCH
	DATA_LOW();
	SCK_LOW();
	
	_delay_us(5);
	
	// Импульс LATCH
	LATCH_HIGH();
	_delay_us(10);
	LATCH_LOW();
	_delay_us(5);
}

static uint8_t reverse8(uint8_t v)
{
    v = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4);
    v = ((v & 0xCC) >> 2) | ((v & 0x33) << 2);
    v = ((v & 0xAA) >> 1) | ((v & 0x55) << 1);
	
	return v;
}

static uint8_t reverse_bits(uint8_t value, uint8_t bits)
{
	return reverse8(value) >> (8 - bits);
}