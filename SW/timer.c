// timer.c

#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "hw.h"
#include "timer.h"


ISR(TIMER0_COMPA_vect)
{
//      Status |= STAT_TIMER;
// tst
        Status |= STAT_STEP;

}


ISR(TIMER1_OVF_vect)
{
        Status |= STAT_ILIM;
}

ISR(TIMER1_COMPA_vect)
{
//              OUT_PORT |= (_BV(OUTBB)|_BV(OUTAB));            // switch off N devices chan A
                OUT_PORT &= ~(_BV(OUTA)|_BV(OUTB));             // switch off P devices chan A
}

ISR(TIMER1_COMPB_vect)
{
//              OUT_PORT |= (_BV(OUTDB)|_BV(OUTCB));            // switch off N devices chan B
                OUT_PORT &= ~(_BV(OUTD)|_BV(OUTC));             // switch off P devices chan B
}

ISR(TIMER2_COMPA_vect)
{
                if(Stat_chop & CHOP_CHOPA)
                {
                        ChopA++;
                        switch(ChopA)
                        {
                                    case 2:
                                                        Stat_chop |= CHOP_FASTA;
                                            break;
                                        case 4:
                                                        Stat_chop &= ~(CHOP_CHOPA | CHOP_FASTA);
                                                        ChopA = 0;
                                                        Status |= STAT_GO;
                                                        break;
                        }
                }
/*              if(Stat_chop & CHOP_CHOPB)
                {
                        ChopB++;
                        switch(ChopB)
                        {
                                    case 2:
                                                        Stat_chop |= CHOP_FASTB;
                                            break;
                                        case 4:
                                                        Stat_chop &= ~CHOP_CHOPB;
                                                        Stat_chop &= ~CHOP_FASTB;
                                                        ChopA = 0;
                                                        Status |= STAT_GO;
                                                        break;
                        }
                }
*/
}


void Timer_Init(void)
{
        TCCR0A = 0;
////    OCR1A = 9766;                   // OCR1A every 500ms
//      OCR1A = 0x4c4c;                 // OCR1A every 1000ms (only 1 byte needed for 4min stand-by delay) 19532
//tst
        OCR0A = 30;                             // OCR1A every 2ms?

        TCNT0 = 0;
        TIMSK0 |= _BV(OCIE0A);  // enable ISR from OCR A and B (tst)
        TIFR0 |= _BV(OCF0A);    // clear potential interrupt
        TCCR0B = 0x0D;                  //CTC mode 4, 1024 prescaler, start counter
        
//      PWM     timer 1
////    TCCR1B = _BV(CS01)|_BV(CS00);           // prescaler 64
////    TCCR1B = _BV(CS01);                                     // prescaler 8
//      ICR1 = 0x0FFF;                                          // 12bit for now running on 20MHz
//      TCCR1B = _BV(CS10)|_BV(WGM12)|_BV(WGM13);                                       // no prescaler 
//      TCCR1A = _BV(WGM01);                            // fast PWM
//      OCR1A = 1;                                                      // minimum current
//      OCR1B = 1;
//      TIMSK1 = _BV(OCIE1B)|_BV(OCIE1A)|_BV(TOIE1);
        
//      Chop timer 2 (CTC mode running at 100kHz?)
        TCNT2 = 0;
        TCCR2A = _BV(WGM21);                            // CTC mode; top = OCR2A
        TCCR2B = _BV(CS22)|_BV(CS21);           // clk/256 = 78.125 kHz   /4 = 19kHz
        OCR2A = 4;
        TIMSK2 |= _BV(OCIE2A);                          // enable ISR from OCR A 

}
