#ifndef HW_H
#define HW_H

#include <inttypes.h>

#ifndef GLUE
	#define GLUE(a, b)     a##b
	#define PORT(x)        GLUE(PORT, x)
	#define PIN(x)         GLUE(PIN, x)
	#define DDR(x)         GLUE(DDR, x)
#endif	//GLUE

#define FOSC				20000
#define LITTLE_ENDIAN
#define STB_DELAY			240			// 4 mins delay to reduce current
#define STEP_PULSE			30			// dead time between switching off the P and switching on N in us.

#define OUT_P				D
#define STEP				PD0
#define DIR					PD2
#define EN					PD1

#define KEY_P
#define KEYN
#define KEYP
#define KEYNF
#define KEYPF

#define KEYFAST				0x04
#define KEYPOS				0x01
#define KEYNEG				0x02

#define STAT_RX				0x01
#define STAT_GO				0x02
#define STAT_SLEEP			0x04
#define STAT_TIMER			0x08
#define STAT_GO_ZERO		0x10
#define STAT_PRINT			0x20
#define STAT_1SEC			0x40
#define STAT_RX_OVF			0x80

#define OUT_DDR				DDR(OUT_P)
#define OUT_PORT			PORT(OUT_P)

#define KEY_DDR				DDR(KEY_P)
#define KEY_PIN				PIN(KEY_P)
#define KEY_PORT			PORT(KEY_P)


extern volatile uint8_t Status;

#endif //HW_H