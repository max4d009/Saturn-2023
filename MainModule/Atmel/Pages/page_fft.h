/*
 * pageFFT.h
 *
 * λ Created: 24.10.2023 18:22:14
 *  Author: m4d
 */ 
#ifndef PAGEFFT_H_
#define PAGEFFT_H_

#include "../main.h"

#define SELECT_PAGE_FFT_CHANNEL 0
#define SELECT_PAGE_FFT_BIAS 1
#define SELECT_PAGE_FFT_UZ_EQ 2
#define SELECT_PAGE_FFT_NUM 3

#define V_BIAS 29

void page_fft_render(uint8_t first_render);
void page_fft_menu();
void page_fft_select();
void page_fft_plus();
void page_fft_minus();
void page_fft_save();

#endif /* PAGEFFT_H_ */