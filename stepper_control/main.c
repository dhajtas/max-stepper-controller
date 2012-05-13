
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
	uint8_t	 dir, key, sw;
	int16_t	 angle = 0;
	int16_t position = 0;

	Status = 0;

#if DEBUG == 1
	DEBUG_DDR = 0xff;		//set debug port as output
	DEBUG_PORT = 0xFF;
#endif

	DRV_Init();
	RS_Init();
	Timer_Init();

	sei();					// start global interrupts

#if DEBUG == 1
	DEBUG_PORT = 0x00;
#endif
	while(1)
	{
#if DEBUG == 1
		DEBUG_PORT ^= 0x80;
#endif

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
					case   	's':
					case 	'S':	Status |= STAT_PRINT;
									RS_Clr();
									break;
//					case 	'z':
//					case 	'Z':	angle = 0;
//									RS_Clr();
//									RS_Send_P(PSTR("OK\n"));
//									break;
				}
			}
		}
		
		if(Status & STAT_GO)
		{
			DEBUG_PORT ^= 0x10;
			if(Status & STAT_TIMER)
			{
				if(steps)
				{
					DRV_Step(dir);
					sw = OUT_PIN & _BV(SWEND);
					if( sw == _BV(SWEND))
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
				}
				if(steps == 0)
				{
					Status &= ~STAT_GO;				// if last step...
					if(!(Status & STAT_MAN))
						RS_Send_P(PSTR("OK\n"));
					Status &= ~STAT_MAN;
				}
			}
		}
		
		if(Status & STAT_PRINT)
		{
//			RS_Send_P(PSTR("Angle         : "));
//			RS_Send_num(angle,1,1);
//			RS_Send_P(PSTR("Abs. position : "));
			RS_Send_P(PSTR("+"));
			RS_Send_num(position);
			Status &= ~STAT_PRINT;
		}
		
		if(Status & STAT_GO_ZERO)
		{
			sw = (OUT_PIN & _BV(SWEND));
			while( sw == _BV(SWEND))
			{
				DRV_Step(0);
				_delay_us(STEP_PULSE*10);
				sw = (OUT_PIN & _BV(SWEND));
			}
			Status &= ~STAT_GO_ZERO;
			position = 0;
			angle = -1350;
			RS_Send_P(PSTR("OK\n"));
		}
		
		key = DRV_Get_key();
		
		if(key)
		{
#if DEBUG == 1
			DEBUG_PORT ^= _BV(DRV_KEYLED);
#endif
			Status |= STAT_MAN;
			switch(key & KEYFAST)
			{
				case 0		:	if(!(Status & STAT_1SEC))
									break;
								Status &= ~STAT_1SEC;
				case KEYFAST:	if(key & KEYPOS)
									dir = 1;
								else
									dir = 0;
								_delay_us(STEP_PULSE*10);
								steps = 1;
								Status |= STAT_GO;
								break;
			}
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

