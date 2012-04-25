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
#define DEAD_TIME			30			// dead time between switching off the P and switching on N in us.

// define what 4 tables will be switched by DIPSW 1:0
#define TABLE0				STEP_1		//full step
#define TABLE1				STEP_2		//half step
#define TABLE2				STEP_4		//quater step
#define TABLE3				STEP_8		//1/8 step
// define table size
#define STEPS0				4			//200 steps/rev
#define STEPS1				8			//400 steps/rev
#define STEPS2				16			//800 steps/rev
#define STEPS3				32			//1600 steps/rev

#define SPI_P				B
#define SPI_MOSI			PB2
#define SPI_MISO			PB3
#define SPI_SS				PB0
#define SPI_SCK				PB1

#define OUT_P				D
#define OUTA				PD0
#define OUTAB				PD2
#define OUTB				PD1
#define OUTBB				PD3
#define OUTC				PD4
#define OUTCB				PD6
#define OUTD				PD5
#define OUTDB				PD7

#define IN_P				C
#define STEP				PC5
#define DIR					PC4
#define DIP0				PC0
#define DIP1				PC1
#define DIP2				PC2
#define DIP3				PC3

#define ISA					7
#define ISB					6

#define STAT_STEP			0x01
#define STAT_GO				0x02
#define STAT_PDN			0x04
#define STAT_TIMER			0x08
#define STAT_ILIM			0x10
#define STAT_FIRST_ADC		0x80

#define CHOP_FASTA			0x01
#define CHOP_FASTB			0x10
#define CHOP_CHOPA			0x02
#define CHOP_CHOPB			0x20

#define IN_DDR				DDR(IN_P)
#define IN_PORT				PORT(IN_P)
#define IN_PIN				PIN(IN_P)

#define OUT_DDR				DDR(OUT_P)
#define OUT_PORT			PORT(OUT_P)

extern volatile uint8_t CurrentA;
extern volatile uint8_t CurrentB;
extern volatile uint8_t Status;
extern volatile uint8_t Stat_chop;
extern volatile uint8_t ChopA;
extern volatile uint8_t ChopB;

#endif //HW_H