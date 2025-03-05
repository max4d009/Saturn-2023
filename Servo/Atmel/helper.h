/*
 * helper.h
 *
 * λ Created: 12.08.2022 2:04:10
 *  Author: m4d
 */ 
#ifndef HELPER_H_
#define HELPER_H_

#include "main.h"

int16_t computePID(uint16_t input, uint16_t target, double kp, double ki, double kd, double dt, int16_t minOut, int16_t maxOut);
void pidReset(uint16_t integral);

#define constrain_ard(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#endif /* HELPER_H_ */