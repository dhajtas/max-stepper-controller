
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
	uint8_t	 dir, key;
	int16_t	 angle;
	uint16_t position;

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
									RS_Send_P(PSTR("OK\n\r"));
									break;
				}
			}
		}
		
		if(Status & STAT_GO)
		{
			if(Status & STAT_TIMER)
			{
				DRV_Step(dir);
				if(SW_END)
				{
					steps--;
					if(dir)
					{
						position++;
						angle += 18;
					}
					else
					{
						position--;
						angle -= 18;
					}
				}
				else
				{
					DRV_Step((dir + 1) & 0x01);		// one step back if end switch was hit
					steps = 0;							// end of move - clear steps
				}
				if(steps == 0)
				{
					Status &= ~STATUS_GO;				// if last step...
					RS_Send_P(PSTR("OK\n\r"));
				}
			}
		}
		
		if(Status & STAT_PRINT)
		{
			RS_Send_P(PSTR("Angle         : /t"));
			RS_Send_num(angle,1,1);
			RS_Send_P(PSTR("Abs. position : /t"));
			RS_Send_num(position,0,1);
		}
		
		if(Status & STAT_GO_ZERO)
		{
			while(SW_END)
			{
				DRV_Step(0);
				_delay_us(PULSE*10);
			}
			Status &= ~STAT_GO_ZERO;
			position = 0;
			angle = -1350;
			RS_Send_P(PSTR("OK\n\r"));
		}
		
		key = DRV_Get_key();
		
		switch(key & KEYFAST)
		{
			case 0		:	if(!(Status & STAT_1SEC))
								break;
			case KEYFAST:	if(key & 0x01)
								DRV_Step(1);
							else
								DRV_Step(0);
							_delay_us(PULSE*10);
							break;
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

