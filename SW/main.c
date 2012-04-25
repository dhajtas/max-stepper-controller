
#include <inttypes.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
//#include <avr/sleep.h>
//#include <avr/eeprom.h>

#include "hw.h"
#include "adc.h"
#include "step_seq.h"
#include "timer.h"

//-------------------------------------
//			Global variables
//-------------------------------------

volatile uint8_t Status, Stat_chop;
volatile uint8_t CurrentA, CurrentB;
volatile uint8_t ChopA, ChopB;

//-------------------------------------
//			Prototypes
//-------------------------------------

void HW_Init(void);
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

//-------------------------------------
//          MAIN
//-------------------------------------

ISR(PCINT1_vect)			// interrupt from STEP
{
//	if(IN_PIN & _BV(STEP))		// if change to 1 detected (rising edge)
//		Status |= STAT_STEP;
}


int main(void)
{
	uint8_t dir, position;
	uint8_t current_setting;
	uint8_t stb_counter;		// only 8 bits - 240 = 4 minutes
	const prog_void *Step_table;
	uint8_t Max_steps;
	uint8_t maxcurrent;
	
	ChopA = 0;
	ChopB = 0;
	Status = 0;
	Stat_chop = 0;
	
	HW_Init();
//tst	
	ADC_Init();
	
//	Step_Init();
	position = IN_PIN;
	switch(position & 0x03)
	{
		case 0: Step_table = TABLE0;
				Max_steps = STEPS0;
				break;
		case 1: Step_table = TABLE1;
				Max_steps = STEPS1;
				break;
		case 2: Step_table = TABLE2;
				Max_steps = STEPS2;
				break;
		case 3: Step_table = TABLE3;
				Max_steps = STEPS3;
				break;
	}

//	current_setting = Current_Init();
	switch((position >> 2) & 0x03)
	{
		case 0:	current_setting = 2;		// full current over R gives 0.625V
				break;
		case 1:	current_setting = 4;		// full current over R gives 1.25V
				break;
		case 2:	current_setting = 8;		// full current over R gives 2.5V
				break;
		case 3:	current_setting = 16;		// full current over R gives 5V
				break;
		default: current_setting = 1;
				break;
	}
	
	Timer_Init();

	sei();					// start global interrupts
	position = 0;
	stb_counter = 0;
	dir = 0;
	maxcurrent=0xFF;
	
	while(1)
	{
		if(Status & STAT_STEP)
		{
			if(IN_PIN & _BV(DIR))
			{
				position++;
				if(position > Max_steps-1)
					position = 0;
			}
			else 
			{
				if(position)
					position--;
				else
					position = Max_steps-1;
			}
		
////			dir = pgm_read_byte(&(Step_table[position].dir));
			dir = pgm_read_byte(Step_table+(position*2));
////			CurrentA = pgm_read_byte(&(Step_table[position].current));
			CurrentA = pgm_read_byte(Step_table + (position*2) + 1);
//tst
			CurrentB = (CurrentA >> 4) * current_setting;
			CurrentA = (CurrentA & 0x0F) * current_setting;

//			if (CurrentA==0)
//				OCR1A = (uint16_t)1<<7;
//			else if (CurrentA >= maxcurrent)
//				OCR1A = (uint16_t)maxcurrent<<4;
//			else
//				OCR1A = (uint16_t)CurrentA<<4;

//			if (CurrentB==0)
//				OCR1B = (uint16_t)1<<7;
//			else if (CurrentB >= maxcurrent)
//				OCR1B = (uint16_t)maxcurrent<<4;
//			else
//				OCR1B = (uint16_t)CurrentB<<4;

			Status |= STAT_GO;
			Status &= ~(STAT_STEP | STAT_PDN);
			Stat_chop = 0;
			ChopA = 0;
			ChopB = 0;
			stb_counter = 0;
		}
		

		if(Status & STAT_GO)
		{
			// Step(dir);									// P(x) - fast ON, slow OFF, N(xB) - slow ON, fast OFF 
			if(dir & 0x01)
			{
				OUT_PORT &= ~(_BV(OUTB));					// first switch off
				OUT_PORT |= _BV(OUTAB);
				_delay_us(DEAD_TIME);
				OUT_PORT &= ~(_BV(OUTBB));					// then switch on new combination
				OUT_PORT |= _BV(OUTA); 
			}
			else
			{
				OUT_PORT &= ~(_BV(OUTA));					// first switch off
				OUT_PORT |= _BV(OUTBB);
				_delay_us(DEAD_TIME);
				OUT_PORT &= ~(_BV(OUTAB));					// then switch on new combination
				OUT_PORT |= _BV(OUTB); 
			}

			if(dir & 0x02)
			{
				OUT_PORT &= ~(_BV(OUTD));					// first switch off
				OUT_PORT |= _BV(OUTCB);
				_delay_us(DEAD_TIME);
				OUT_PORT &= ~(_BV(OUTDB));					// then switch on new combination
				OUT_PORT |= _BV(OUTC); 
			}
			else
			{
				OUT_PORT &= ~(_BV(OUTC));					// first switch off
				OUT_PORT |= _BV(OUTDB);
				_delay_us(DEAD_TIME);
				OUT_PORT &= ~(_BV(OUTCB));					// then switch on new combination
				OUT_PORT |= _BV(OUTD); 
			} 
			Status &= ~STAT_GO;
		}

/*		if(Status & STAT_ILIM)
		{
			// Step(dir);									// P(x) - fast ON, slow OFF, N(xB) - slow ON, fast OFF 
			if(dir & 0x01)
			{
				OUT_PORT |= (_BV(OUTA));					// then switch on new combination
			}
			else
			{
				OUT_PORT |= (_BV(OUTB));					// then switch on new combination
			}

			if(dir & 0x02)
			{
				OUT_PORT |= (_BV(OUTC));					// then switch on new combination
			}
			else
			{
				OUT_PORT |= (_BV(OUTD));					// then switch on new combination
			}
			Status &= ~STAT_ILIM;
		}
*/

		if(Stat_chop & CHOP_CHOPA)
		{
			if(Stat_chop & CHOP_FASTA)						// fast decay
			{
				if(dir & 0x01)
				{
					OUT_PORT |= _BV(OUTBB);
//					OUT_PORT |= _BV(OUTB); 
				}
				else
				{
					OUT_PORT |= _BV(OUTAB);
//					OUT_PORT |= _BV(OUTA); 
				}
			}
			else											// slow decay
			{
				OUT_PORT &= ~(_BV(OUTA)|_BV(OUTB));	// switch to slow decay - first switch P off
				_delay_us(DEAD_TIME);
				OUT_PORT &= ~(_BV(OUTAB)|_BV(OUTBB));	// then switch on N 
			}
		}

		if(Stat_chop & CHOP_CHOPB)
		{
			if(Stat_chop & CHOP_FASTB)						// fast decay
			{
				if(dir & 0x02)
				{
					OUT_PORT |= _BV(OUTDB);
//					OUT_PORT |= _BV(OUTD); 
				}
				else
				{
					OUT_PORT |= _BV(OUTCB);
//					OUT_PORT |= _BV(OUTC); 
				}
			}
			else											// slow decay
			{
				OUT_PORT &= ~(_BV(OUTC) | _BV(OUTD));		// first switch off
				_delay_us(DEAD_TIME);
				OUT_PORT &= ~(_BV(OUTCB) | _BV(OUTDB));	// then switch on new combination
			}
		} 
		
		if(Status & STAT_TIMER)						// if timer isr
		{
			if(!(Status & STAT_PDN))				// if not in power save
			{
				stb_counter++;
				if(stb_counter > STB_DELAY)
				{
					Status |= STAT_PDN;
					CurrentA = CurrentA >> 1;		//reduce current in standby mode to 1/2
					CurrentB = CurrentB >> 1;		//reduce current in standby mode to 1/2
				}
			}
			Status &= ~(STAT_TIMER);				// clear flag
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

