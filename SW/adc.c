#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "hw.h"
#include "adc.h"

/*---------------------------------------------------------------------------*/
/*		ADC interrup routine												 */
/*---------------------------------------------------------------------------*/

//#if ADC_INT_ENABLE == 1

//volatile int16_t ADC_Buffer[10];
//volatile uint16_t ADC_Index;
volatile uint8_t Channel;

ISR(ADC_vect)
{
	uint8_t i;
	
	i = ADCH;					//read upper 8 bits only
	ADMUX = _BV(ADLAR) | _BV(REFS1) | _BV(REFS0) | Channel;

	if(Channel == ISA)
//	if(!(Stat_chop & CHOP_CHOPA))
	{
		Channel = ISB;
		if(!(Stat_chop & CHOP_CHOPA))
			if(i >= CurrentA)
			{	// ~A and ~B are inverted!!!
				Stat_chop |= CHOP_CHOPA;
			}
	}

	if(Channel == ISB)
	{
		Channel = ISA;
		if(!(Stat_chop & CHOP_CHOPB))
			if(i >= CurrentB)
			{	// ~C and ~D are inverted!!!
				Stat_chop |= CHOP_CHOPB;
			}
	}
}

void ADC_Init(void)
{

	ADCSRA = _BV(ADEN)| 0x05;						// enables ADC by setting bit 7 (ADEN) in the ADCSRA, full speed
											// clk/2 - 769.6kSps ??
	ADCSRB = 0x00;							// free running mode
//	ADMUX = _BV(ADLAR) | ISA;		// selects single-ended conversion on pin channel 7, left adjusted, VREF pin as reference (VCC via 22uH choke)
	ADMUX = _BV(ADLAR) | _BV(REFS1) | _BV(REFS0) | ISA;		// selects single-ended conversion on pin channel 7, left adjusted, internal 1.1V reference 
	Channel = ISA;

	ADCSRA |= _BV(ADIE) | _BV(ADIF) | _BV(ADATE);		// enable interrupt, clear old one if exists 
	ADCSRA |= _BV(ADSC);								// first conversion needs to be started ...
	sei();
	ADMUX = _BV(ADLAR) | ISB;
}

