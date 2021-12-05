#include "xc.h"

void ComparatorInit(void) {
    
    TRISBbits.TRISB1 = 1;
    AD1PCFGbits.PCFG3 = 0;
    
    
}