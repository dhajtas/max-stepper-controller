
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
#include "drv.h"
#include "timer.h"

//-------------------------------------
//			Global variables
//-------------------------------------

volatile uint8_t Status;

//-------------------------------------
//			Prototypes
//-------------------------------------

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

//-------------------------------------
//          MAIN
//-------------------------------------



int main(void)
{
	uint16_t steps = 0;
	uint8_t	 dir, key;
	int16_t	 angle = 0;
	uint16_t position = 0;

	Status = 0;
	
	DRV_Init();
	RS_Init();
	Timer_Init();

	sei();					// start global interrupts
	
	while(1)
	{
		if(Status & STAT_RX)
		{
			if(Status & STAT_RX_OVF)
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
				if((OUT_PIN & _BV(SWEND)) == _BV(SWEND))
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
					Status &= ~STAT_GO;				// if last step...
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
			while((OUT_PIN & _BV(SWEND)) == _BV(SWEND))
			{
				DRV_Step(0);
				_delay_us(STEP_PULSE*10);
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
								dir = 1;
							else
								dir = 0;
							_delay_us(STEP_PULSE*10);
							steps = 1;
							Status |= STAT_GO;
							break;
		}

	}

	return(0);
}


void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();
    return;
}

