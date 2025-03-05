/*
 * Created: 12.08.2022 2:03:38
 *  Author: m4d
 */ 

#include "helper.h"

long double integral = 0, prevErr = 0;

void pidReset(uint16_t integral_)
{
	integral = integral_;
	prevErr = 0;
}

// тоже перенести
int16_t computePID(uint16_t input, uint16_t target, double kp, double ki, double kd, double dt, int16_t minOut, int16_t maxOut)
{
	int16_t err = target - input;
	integral = constrain_ard(integral + (double)err * dt * ki, minOut, maxOut);
	long double D = (err - prevErr) / dt;
	prevErr = err;
	return constrain_ard(err * kp + integral + D * kd, minOut, maxOut);
}