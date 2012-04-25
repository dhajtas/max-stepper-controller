//------------------------------------------------------------
//			Serial comm module
//------------------------------------------------------------

#ifndef RS_H
#define RS_H


//------------------------------------------------------------
//			Global public variables
//------------------------------------------------------------

//------------------------------------------------------------
//			Public prototypes
//------------------------------------------------------------


void RS_Init(void);

void RS_Send8_t(uint8_t tx_data);

uint8_t RS_Send_P(PGM_P tx_string);

uint8_t RS_Get8_t(void);

void RS_Clr(void);

uint16_t RS_Getnum(void);

#endif		//RS_H