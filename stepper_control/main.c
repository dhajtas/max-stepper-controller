
#include <inttypes.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
//#include <avr/sleep.h>
//#include <avr/eeprom.h>

#include "hw.h"
#include "rs.h"
#include "timer.h"

//-------------------------------------
//			Global variables
//-------------------------------------

volatile uint8_t Status;

//-------------------------------------
//			Prototypes
//-------------------------------------

void HW_Init(void);
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

//-------------------------------------
//          MAIN
//-------------------------------------



int main(void)
{
	uint16_t steps;
	uint8_t	 dir;
	int16_t	 angle;

	Status = 0;
	
	HW_Init();
	Timer_Init();

	sei();					// start global interrupts
	
	while(1)
	{
		if(Status & STAT_RX)
		{
			if(Satus & STAT_RX_OVF)
			{
			}
			else
			{
				switch(RS_Get8_t())
				{
					case 	'+':	dir = 1;
									steps = RS_Getnum();
									Status |= STAT_GO;
									break;
					case 	'-':	dir = 0;
									steps = RS_Getnum();
									Status |= STAT_GO;
									break;
					case 	'0':	Status |= STAT_GO_ZERO;
									RS_Clr();
									break;
					case   	't':
					case 	'T':	Status |= STAT_PRINT;
									RS_Clr();
									break;
					case 	'z':
					case 	'Z':	angle = 0;
									RS_Clr();
									break;
				}
			}
		}

	}

	return(0);
}

void HW_Init(void)
{
	//clk 20 MHZ - set the system clk prescaler to 0
	CLKPR = _BV(CLKPCE);		// enable change
	CLKPR = 0x00;				// div by 1
	
	IN_DDR = 0;					// all pins as inputs
	IN_PORT = (_BV(STEP)|_BV(DIR)|_BV(DIP0)|_BV(DIP1)|_BV(DIP2)|_BV(DIP3)); 	//internal pull-ups enabled
	
	OUT_DDR = 0xFF;				// all pins as outputs
	OUT_PORT = 0;				// everything off 
	
	PCMSK1 |= _BV(PCINT13);		// unmask pcint13 - STEP signal
	PCICR |= _BV(PCIE1)	;		// enable pin change interrupt from 8:14
	
}


void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();
    return;
}

