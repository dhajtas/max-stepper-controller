//	step_seq.h
#ifndef STEP_H
#define STEP_H

#ifndef GLUE
	#define GLUE(a, b)     a##b
	#define PORT(x)        GLUE(PORT, x)
	#define PIN(x)         GLUE(PIN, x)
	#define DDR(x)         GLUE(DDR, x)
#endif	//GLUE



typedef struct SEQ {
	uint8_t dir;
	uint8_t current;
} seq_t;

extern seq_t PROGMEM STEP_1_WAVE[];
extern seq_t PROGMEM STEP_1[];
extern seq_t PROGMEM STEP_2[];
extern seq_t PROGMEM STEP_4[];
extern seq_t PROGMEM STEP_8[];
extern seq_t PROGMEM STEP_16[];

#endif // STEP_H