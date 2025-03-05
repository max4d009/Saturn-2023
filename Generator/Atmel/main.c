/*
 * gen.c
 *
 * λ Created: 27.08.2021 10:42:14
 * Author : m4d
 */ 

#include "main.h"

void set_freq(int freq_)
{
	SPI_init(); // todo: проверить без этой строки
	Freq_change(freq_, 0);
}

int main(void)
{
	sei();
	
	m4d_i2c_init_as_slave();
	SPI_init();
	AD9833_init();

    while (1) 
    {
		execute_command_timer();
    }
}

