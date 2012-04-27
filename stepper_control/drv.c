//------------------------------------------------------------
//			PHY driver module
//------------------------------------------------------------

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>

#include "hw.h"

void DRV_Init(void)
{
		OUT_DDR |= (_BV(STEP) | _BV(DIR) | _BV(EN));
		OUT_PORT &= ~(_BV(STEP) | _BV(DIR));
		OUT_PORT |= _BV(EN);
		KEY_DDR &= ~(_BV(KEYNF) | _BV(KEYN) | _BV(KEYP) | _BV(KEYPF));		//key as inputs
		KEY_PORT |= (_BV(KEYNF) | _BV(KEYN) | _BV(KEYP) | _BV(KEYPF));		//enable pull-ups
}


void DRV_Step(uint8_t dir);
{
	if(dir)
		OUT_PORT |= _BV(DIR);
	else
		OUT_PORT ~= ~(_BV(DIR));
	
	OUT_PORT |= _BV(STEP);
	_delay_us(PULSE);
	OUT_PORT &= ~(_BV(STEP));
}

uint8_t DRV_Get_key(void)
{
	uint8_t input;
	
	input = KEY_PIN;
	if(input & _BV(KEYP))
		return(KEYPOS);

	if(input & _BV(KEYN))
		return(KEYNEG);

	if(input & _BV(KEYPF))
		return(KEYPOS | KEYFAST);

	if(input & _BV(KEYNF))
		return(KEYNEG | KEYFAST);
		
	return(0);
}