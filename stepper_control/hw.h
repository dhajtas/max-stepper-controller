#ifndef HW_H
#define HW_H

#include <inttypes.h>

#ifndef GLUE
	#define GLUE(a, b)     a##b
	#define PORT(x)        GLUE(PORT, x)
	#define PIN(x)         GLUE(PIN, x)
	#define DDR(x)         GLUE(DDR, x)
#endif	//GLUE

#define DEBUG 				1
#define FOSC				11059
#define LITTLE_ENDIAN
#define STB_DELAY			240			// 4 mins delay to reduce current
#define STEP_PULSE			30			// Pulse width in us.
#define RX_BUFF_SIZE		10

#define DISP_P				C
#define DISP_E				PC3
#define DISP_RS				PC2
#define DISP_D4				PC4
#define DISP_D5				PC5
#define DISP_D6				PC6
#define DISP_D7				PC7

#if DEBUG == 1
	#define DEBUG_P			B
	#define TIMER_TICKLED	PB0
	#define RS_RXLED		PB1
	#define RS_TXLED		PB2
	#define DRV_KEYLED		PB3
#endif

#define OUT_P				D
#define STEP				PD0
#define DIR					PD2
#define EN					PD1
#define SWEND				PD3

#define RS_RX				PE0
#define RS_TX				PE1

#define KEY_P				A
#define KEYN				PA2
#define KEYP				PA1
#define KEYNF				PA3
#define KEYPF				PA0

#define KEYFAST				0x04
#define KEYPOS				0x01
#define KEYNEG				0x02

#define STAT_RX				0x01
#define STAT_GO				0x02
#define STAT_MAN			0x04
#define STAT_TIMER			0x08
#define STAT_GO_ZERO		0x10
#define STAT_PRINT			0x20
#define STAT_1SEC			0x40
#define STAT_RX_OVF			0x80

#define OUT_DDR				DDR(OUT_P)
#define OUT_PORT			PORT(OUT_P)
#define OUT_PIN				PIN(OUT_P)

#define KEY_DDR				DDR(KEY_P)
#define KEY_PIN				PIN(KEY_P)
#define KEY_PORT			PORT(KEY_P)

#define DEBUG_DDR			DDR(DEBUG_P)
#define DEBUG_PORT			PORT(DEBUG_P)
#define DEBUG_PIN			PIN(DEBUG_P)

extern volatile uint8_t Status;

#endif //HW_H