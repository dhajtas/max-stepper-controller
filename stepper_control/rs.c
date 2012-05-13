//------------------------------------------------------------
//			Serial comm module
//------------------------------------------------------------

#include <inttypes.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "hw.h"

//------------------------------------------------------------
//			Global variables
//------------------------------------------------------------

volatile uint8_t RX_Index;
volatile uint8_t RX_Buffer[RX_BUFF_SIZE];

//------------------------------------------------------------
//			Local prototypes
//------------------------------------------------------------

void RS_Clr(void);

//------------------------------------------------------------
//			Interrupt routines
//------------------------------------------------------------

ISR(USART0_RX_vect)
{
	uint8_t data;

	data = UDR0;
	
	switch(data)
	{
		case 0x0D: Status |= STAT_RX;
		case 0x0A: break;
		default:
#if DEBUG == 1
					DEBUG_PORT |= _BV(RS_RXLED);
#endif
					RX_Buffer[RX_Index++] = data;
					if (RX_Index > RX_BUFF_SIZE)
					{
						Status |= STAT_RX_OVF;
						RX_Index = 0;
					}
					break;
	}
}




void RS_Init(void)
{
	UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	UBRR0L = 71;									// 9600,N,1 @ 11.0592MHz
	RS_Clr();
}

void RS_Send8_t(uint8_t tx_data)
{
#if DEBUG == 1
	DEBUG_PORT |= _BV(RS_TXLED);
#endif

	while(!(UCSR0A & _BV(UDRE0)));
	UDR0 = tx_data;

#if DEBUG == 1
	DEBUG_PORT &= ~(_BV(RS_TXLED));
#endif

}

uint8_t RS_Send_P(PGM_P tx_string)		//, uint8_t eol)
{
	uint8_t lenght = strlen_P(tx_string);
	uint8_t i, chr;
	
	for(i=0;i<(lenght-1);i++)
	{
		chr = pgm_read_byte(tx_string+i);
		if(chr == '\n')
		{
			RS_Send8_t(0x0A);
			RS_Send8_t(0x0D);
		}
		else
		{
			RS_Send8_t(chr);
		}
	}
//	if(eol)
//	{
//		RS_Send8_t(0x0A);
//		RS_Send8_t(0x0D);
//		i+=2;
//	}
	return(i);
}

/*
uint8_t RS_Send_num(int16_t num, uint8_t dec, uint8_t last)
{
	uint8_t decimal;
	uint8_t i=0;
	
	if(num/10)
	{
		if(dec)
			i = RS_Send_num(num/10,dec-1,0);		// recursion!!!
		else
			i = RS_Send_num(num/10,0,0);
	}
	decimal = (num%10) + 48;					// conversion to ASCII
	if(dec == 1)
	{
		RS_Send8_t('.');
		i++;
	}
	RS_Send8_t(decimal);
	i++;
	if(last)
	{
		RS_Send8_t(0x0A);
		RS_Send8_t(0x0D);
		i+=2;
	}
	return(i);
}
*/

uint8_t RS_Send_num(int16_t num)
{
	uint8_t decimal;
	uint8_t i=0;
	
	RS_Send8_t('+');
	RS_Send8_t((num/100) + 48);					// conversion to ASCII
	num = num%100;
	RS_Send8_t((num/10) + 48);
	num = num%10;
	RS_Send8_t(num + 48);
	RS_Send8_t(0x0A);
	RS_Send8_t(0x0D);
	i+=2;
	return(i);
}


uint8_t RS_Get8_t(void)
{
	static uint8_t index = 0;
	uint8_t data;
	
	data = RX_Buffer[index++];
	
	if(index == RX_Index)
	{
		Status &= ~STAT_RX;
		index = 0;
		RS_Clr();
	}
	return(data);
}

uint16_t RS_Getnum(void)
{
	uint16_t data;
	
	data = ((uint16_t)RS_Get8_t()-48)*100;
	data += ((uint16_t)RS_Get8_t()-48)*10;
	data += (RS_Get8_t()-48);
	RS_Clr();
	return(data);
}

void RS_Clr(void)
{
	RX_Index = 0;
	Status &= ~(STAT_RX | STAT_RX_OVF);
#if DEBUG == 1
	DEBUG_PORT &= ~(_BV(RS_RXLED));
#endif
}