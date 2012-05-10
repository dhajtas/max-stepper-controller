// timer.c

#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "hw.h"
#include "timer.h"

volatile uint8_t counter;

ISR(TIMER0_COMPA_vect)
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
	TCCR0A = 0;
	OCR0A = 200;				// OCR1A every 20ms?

	TCNT0 = 0;
	TIMSK0 |= _BV(OCIE0A);	// enable ISR from OCR A 
	TIFR0 |= _BV(OCF0A);	// clear potential interrupt
	TCCR0B = 0x0D;			//CTC mode 4, 1024 prescaler, start counter
}