// timer.c

#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "hw.h"
#include "timer.h"

volatile uint8_t counter;

ISR(TIMER1_COMPA_vect)
{
	Status |= STAT_TIMER;
	if(!(Status & STAT_1SEC))
	{
		counter++;
		if(counter > 50)
		{
			Status | STAT_1SEC;
			counter = 0;
		}
	}
}


void Timer_Init(void)
{
	counter = 0;
	TCCR1A = 0;
	OCR1A = 200;				// OCR1A every 20ms?

	TCNT1 = 0;
	TIMSK |= _BV(OCIE1A);	// enable ISR from OCR A 
	TIFR |= _BV(OCF1A);	// clear potential interrupt
	TCCR1B = 0x0D;			//CTC mode 4, 1024 prescaler, start counter
}