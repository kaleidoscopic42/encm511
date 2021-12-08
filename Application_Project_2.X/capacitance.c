#include "xc.h"

void ComparatorInit(void);
void TimerInit(void);
int measureFrequency(void);
void comparatorGo(void);

int timerActive;
int eventCount2;

void measureCapactitance(void){
  
  TimerInit();
  ComparatorInit();
  eventCount2 = 0;
    CM1CONbits.CON = 1; 	// Turn Comparator ON
    T1CONbits.TON=1;
    
    timerActive = 0;
    
    while(!timerActive){};
    
    unsigned float capacitance = ( (CMSTATbits.C1OUT) * (1/eventCount2) ) / [Voltage Reading])
      
    return capacitance;
}
