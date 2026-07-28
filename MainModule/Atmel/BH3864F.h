/*
 * BH3864F.h
 *
 * Created: 13.06.2026 3:15:04
 *  Author: max4d
 */ 
#ifndef BH3864F_H_
#define BH3864F_H_

#include "main.h"

#define BH3864_LEFT_CHANNEL 0
#define BH3864_RIGHT_CHANNEL 1

#define BH3864_INIT_VOL_DB -19

#define BH_PORT PORTB 
#define BH_DDR DDRB 
#define BH_MOSI PB3 
#define BH_SCK PB5 
#define BH_LATCH PD6

#define DATA_HIGH()  (BH_PORT |= (1 << BH_MOSI)) 
#define DATA_LOW()   (BH_PORT &= ~(1 << BH_MOSI)) 
#define SCK_HIGH()   (BH_PORT |= (1 << BH_SCK)) 
#define SCK_LOW()    (BH_PORT &= ~(1 << BH_SCK)) 
#define LATCH_HIGH() (PORTD   |= (1 << BH_LATCH)) 
#define LATCH_LOW()  (PORTD   &= ~(1 << BH_LATCH))

#define BH3864_VOL_DB_MIN    81
#define BH3864_VOL_A_MAX    30
#define BH3864_VOL_B_MAX_BEFORE_LIMIT    1
#define BH3864_VOL_B_MAX_AFTER_LIMIT  3
#define BH3864_VOL_B_LIMIT  21

typedef struct {
	uint8_t volA_ch1_id;       // D19-D23 (0-31, 31 = MUTE)
	uint8_t volB_ch1_id;       // D24-D25 (0-3)
	uint8_t volA_ch2_id;       // D12-D16 (0-31, 31 = MUTE)
	uint8_t volB_ch2_id;       // D17-D18 (0-3)
	uint8_t bass_id;           // D4-D7 (0-11)
	uint8_t treble_id;         // D8-D11 (0-11)
	uint8_t tbass_id;          // D1-D3 (0-4)
	uint8_t gain_id;           // D0 (0=14dB, 1=11dB)
} bh3864_settings;

const bh3864_settings* bh3864_get_settings(void);
void bh3864_init(void);
void bh3864_set_volume_db(int8_t db);
void bh3864_set_channel_volume_db(int8_t ch1_db, int8_t ch2_db);
void bh3864_set_bass_db(int8_t db);
void bh3864_set_treble_db(int8_t db);
void bh3864_set_tbass_db(uint8_t db);

#endif /* BH3864F_H_ */