#include "counter.h"

#define TPS_MID     60

typedef struct {
	uint8_t ratio;  // L/R * 100 (заранее посчитано)
	uint8_t tps;
} calib_point_t;

static const calib_point_t calib_table[] = {
	// ========================================
	// СЕРЕДИНА ЛЕНТЫ (L ≈ R)
	// ========================================
	{ 100, 56 },  // 9/9   → точное значение
	{ 96,  56 },  // 9/10  → между (9/9=100) и (8/9=88): 56+(57-56)*(96-100)/(88-100) = 56+1*0.33 = 56.3 → 56
	{ 92,  57 },  // 9/11  → между (9/9=100) и (8/9=88): 56+(57-56)*(92-100)/(88-100) = 56+1*0.67 = 56.7 → 57
	{ 88,  57 },  // 8/9   → точное значение
	{ 84,  58 },  // 8/10  → между (8/9=88) и (8/10=80): 57+(59-57)*(84-88)/(80-88) = 57+2*0.5 = 58 ✅
	{ 80,  59 },  // 8/10  → точное значение
	{ 76,  60 },  // 8/11  → между (8/10=80) и (8/11=72): 59+(61-59)*(76-80)/(72-80) = 59+2*0.5 = 60 ✅
	{ 72,  61 },  // 8/11  → точное значение
	{ 69,  62 },  // 8/12  → между (8/11=72) и (7/12=58): 61+(64-61)*(69-72)/(58-72) = 61+3*0.21 = 61.6 → 62
	{ 66,  63 },  // 8/12  → между (8/11=72) и (7/12=58): 61+(64-61)*(66-72)/(58-72) = 61+3*0.43 = 62.3 → 62

	// ========================================
	// ПРИБЛИЖЕНИЕ К КРАЮ (L < R)
	// ========================================
	{ 58,  64 },  // 7/12  → точное значение
	{ 56,  65 },  // 7/12.5 → между (7/12=58) и (7/13=53): 64+(66-64)*(56-58)/(53-58) = 64+2*0.4 = 64.8 → 65
	{ 53,  66 },  // 7/13  → точное значение
	{ 52,  67 },  // 7/13.5 → между (7/13=53) и (7/14=50): 66+(69-66)*(52-53)/(50-53) = 66+3*0.33 = 67 ✅
	{ 50,  69 },  // 7/14  → точное значение
	{ 48,  70 },  // 7/14.5 → между (7/14=50) и (7/15=46): 69+(71-69)*(48-50)/(46-50) = 69+2*0.5 = 70 ✅
	{ 46,  71 },  // 7/15  → между (7/14=50) и (7/16=43): 69+(73-69)*(46-50)/(43-50) = 69+4*0.57 = 71.3 → 71
	{ 43,  73 },  // 7/16  → точное значение
	{ 41,  74 },  // 7/16.5 → между (7/16=43) и (7/17=40): 73+(75-73)*(41-43)/(40-43) = 73+2*0.67 = 74.3 → 74
	{ 40,  75 },  // 7/17  → между (7/16=43) и (7/19=36): 73+(78-73)*(40-43)/(36-43) = 73+5*0.43 = 75.1 → 75
	{ 38,  76 },  // 7/18  → между (7/17=40) и (7/19=36): 75+(78-75)*(38-40)/(36-40) = 75+3*0.5 = 76.5 → 76
	{ 36,  78 },  // 7/19  → точное значение
	{ 34,  80 },  // 7/20  → экстраполяция: 78+(78-75)*(34-36)/(36-40) = 78+3*0.5 = 79.5 → 80
	{ 32,  82 },  // 7/21  → экстраполяция: 78+(78-75)*(32-36)/(36-40) = 78+3*1 = 81 → 81
	{ 30,  84 },  // 7/22  → экстраполяция: 78+(78-75)*(30-36)/(36-40) = 78+3*1.5 = 82.5 → 83
};

#define CALIB_COUNT (sizeof(calib_table) / sizeof(calib_point_t))

static uint8_t find_tps(uint8_t L, uint8_t R)
{
	if (L < 1 || R < 1) return TPS_MID;
	
	// Всегда берем меньшее/большее (ratio <= 100)
	uint16_t ratio;
	if (L > R) {
		ratio = ((uint32_t)R * 100) / L;  // Инвертируем!
		} else {
		ratio = ((uint32_t)L * 100) / R;
	}
	
	// Поиск в таблице
	uint8_t best_idx = 0;
	uint16_t best_diff = 0xFFFF;
	
	for (uint8_t i = 0; i < CALIB_COUNT; i++) {
		uint16_t diff = (ratio > calib_table[i].ratio) ?
		(ratio - calib_table[i].ratio) :
		(calib_table[i].ratio - ratio);
		
		if (diff < best_diff) {
			best_diff = diff;
			best_idx = i;
		}
	}
	
	return calib_table[best_idx].tps;
}

static uint8_t get_tps(void)
{
	
	static uint8_t LL = 0;
	static uint8_t RR = 0;
	static uint8_t PREV_LL = 0;
	static uint8_t PREV_RR = 0;
	// Берем сырые данные БЕЗ ФИЛЬТРА
	 LL = LL + current.reel_speed_left;
	 RR = RR + current.reel_speed_right;
	
	if (LL < 30 || RR < 30) {
		if (PREV_LL == 0 || PREV_RR == 0) {
			return TPS_MID;
		} else {
			return find_tps(PREV_LL, PREV_RR);
		}
	}

	uint8_t tps = find_tps(LL, RR);
	
	PREV_LL = LL;
	PREV_RR = RR;
	LL = 0;
	RR = 0;
	
	return tps;
}

void timer_reset()
{
	timer.m_sec = 0;
	timer.sec = 0;
	timer.min = 0;
	timer.hour = 0;
	timer.buf_up = 0;
	timer.buf_down = 0;
	timer.all_sec = 0;
	timer.tps = TPS_MID;
}

void counterIn(uint16_t count)
{
	if (count == 0) return;
	if (timer.buf_up < 65000 && count < 500) {
		timer.buf_up += count;
	}
}

void counterOut(uint16_t count)
{
	if (count == 0) return;
	if (timer.buf_down < 65000 && count < 500) {
		timer.buf_down += count;
	}
}

void inc_counter()
{
	static uint16_t remainder_up = 0;
	static uint16_t remainder_down = 0;
	uint16_t chunk_up = 0;
	uint16_t chunk_down = 0;
	uint32_t total_ticks_up;
	uint32_t total_ticks_down;

	timer.tps = get_tps();

	// === Обработка увеличения ===
	if (timer.buf_up > 0) {
		total_ticks_up = ((uint32_t)timer.buf_up * 10) + remainder_up;
		chunk_up = total_ticks_up / timer.tps;
		remainder_up = total_ticks_up % timer.tps;
		timer.buf_up = 0;
		
		for (uint16_t i = 0; i < chunk_up; i++) {
			timer.m_sec++;

			if (timer.m_sec == 10) {
				timer.sec++;
				timer.m_sec = 0;
				if (timer.all_sec < 30000) {
					timer.all_sec++;
				}
				if (timer.sec == 60) {
					timer.min++;
					timer.sec = 0;
					if (timer.min == 60) {
						timer.hour++;
						timer.min = 0;
						if (timer.hour >= 5) {
							timer.hour = 0;
						}
					}
				}
			}
			
// 			char str_buf[10];
// 			oled_printf(76, 18, FONTID_10X16F, strcpy_P(str_buf, PSTR("%02d.%1d")), timer.sec, timer.m_sec);
// 			disp1color_UpdateFromBuff();
		}
	}
	
	// === Обработка уменьшения ===
	if (timer.buf_down > 0) {
		total_ticks_down = ((uint32_t)timer.buf_down * 10) + remainder_down;
		chunk_down = total_ticks_down / timer.tps;
		remainder_down = total_ticks_down % timer.tps;
		timer.buf_down = 0;
		
		for (uint16_t i = 0; i < chunk_down; i++) {
			if (timer.m_sec > 0) {
				timer.m_sec--;
			} else {
				if (timer.all_sec > -30000) {
					timer.all_sec--;
				}
				timer.m_sec = 9;
				
				if (timer.sec > 0) {
					timer.sec--;
				} else {
					timer.sec = 59;
					
					if (timer.min > 0) {
						timer.min--;
					} else {
						timer.min = 59;
						
						if (timer.hour > 0) {
							timer.hour--;
						} else {
							timer.hour = 5;
							timer.min = 59;
							timer.sec = 59;
							timer.m_sec = 9;
						}
					}
				}
			}
		}
	}
}