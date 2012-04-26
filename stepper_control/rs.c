//------------------------------------------------------------
//			Serial comm module
//------------------------------------------------------------

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "hw.h"

//------------------------------------------------------------
//			Global variables
//------------------------------------------------------------

volatile uint8_t RX_Index;
volatile uint8_t RX_Beuffer[RX_BUFF_SIZE];

//------------------------------------------------------------
//			Local prototypes
//------------------------------------------------------------


//------------------------------------------------------------
//			Interrupt routines
//------------------------------------------------------------

ISR(USART0_RX_vect)
{
	uint8_t data = UDR0;
	
	switch(data)
	{
		case 0x0D: Status |= STAT_RX;
		case 0x0A: break;
		default:
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

	RS_Clr();
}

void RS_Send8_t(uint8_t tx_data)
{
	While(!(UCSR0A & _BV(UDRE0)));
	UDR0 = tx_data;
}

uint8_t RS_Send_P(PGM_P tx_string)
{
	uint8_t lenght = strlen(tx_string);
	uint8_t i;
	
	for(i=0;i++;i<(lenght-1))
	{
		RS_Send8_t(pgm_read_byte(tx_string+i));
	}
	RS_Send8_t(0x0A);
	RS_Send8_t(0x0D);
	i+=2;
	return(i);
}

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
		i += RS_Send8_t('.');
	i =+ RS_Send8_t(decimal);
	if(last)
	{
		RS_Send8_t(0x0A);
		RS_Send8_t(0x0D);
		i+=2;
	}
	return(i);
}

uint8_t RS_Get8_t(void)
{
	static uint8_t index = 0;
	uint8_t data
	
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
	
	data = ((uint16_t)RS_Get8_t()-48)*1000;
	data += ((uint16_t)RS_Get8_t()-48)*100;
	data += (RS_Get8_t()-48)*10;
	RS_Clr();
	return(data);
}

void RS_Clr(void)
{
	RX_Index = 0;
	Status &= ~(STAT_RX | STAT_RX_OVF);
}