#include "xc.h"

unsigned int CTMUcalibrate(void) {
    
    CTMUCONbits.CTMUEN = 0;             // Disable the CTMU
    CTMUCONbits.TGEN = 0;               // Configured for current generation
    CTMUCONbits.EDGEN = 0;              // Edges are disabled
    CTMUCONbits.IDISSEN = 0;            // Current source is grounded (discharge enabled)
    CTMUICONbits.ITRIM = 0b000000;      // No trim
    CTMUCONbits.CTTRIG = 0;             // Trigger output disabled
    
    CTMUICONbits.IRNG = 0b11;             // 100 x base current
    
    TRISBbits.TRISB1 = 1;
    AD1PCFG = 0xFFF7;
    AD1CHSbits.CH0SA = 0b00011;         // Positive input is AN5/RA3/pin8 for MUXA
    
    AD1CON1 = 0x8000;
    AD1CON2 = 0x0000;
    AD1CON3 = 0x0000;
    
    CTMUCONbits.CTMUEN = 1;             // Enabled the CTMU
    CTMUCONbits.EDG1STAT = 1;           // Enable current source
    AD1CON1bits.SAMP = 1;               // Manual sampling start
    
    // Delay to discharge sample cap
    unsigned int x;
    for(x=0; x<2000; x++);
    
    IFS0bits.AD1IF = 0;                 // Clear ADC interrupt
    
    AD1CON1bits.SAMP = 0;               // Begin A/D conversion
    while(AD1CON1bits.DONE == 0);        // Wait for A/D conversion to complete
        
    CTMUCONbits.EDG1STAT = 0;          // Disbale current source
    IFS0bits.AD1IF = 0;                 // Clear ADC interrupt
            
    unsigned int result = ADC1BUF0;
    
    CTMUCONbits.CTMUEN = 0;                 //  Disable the CTMU
    
    return result;
           
}