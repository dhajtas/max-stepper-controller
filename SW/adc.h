#ifndef __ADC_H__
#define __ADC_H__

//----------------------------------------------------------
//						DEFS
//----------------------------------------------------------

#ifndef GLUE
	#define GLUE(a, b)     a##b
	#define PORT(x)        GLUE(PORT, x)
	#define PIN(x)         GLUE(PIN, x)
	#define DDR(x)         GLUE(DDR, x)
#endif	//GLUE

#define ADC_PORT	PORT(ADC_P)
#define ADC_DDR		DDR(ADC_P)

void ADC_Init(void);



#endif //#ifndef __ADC_H__