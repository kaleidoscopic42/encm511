#include "xc.h"

int eventCount = 0;

void Comparator2Init(void) {
    
    IEC1bits.CMIE = 0;          // Disable interrupts while setting up pin
    
    TRISAbits.TRISA2 = 1;       // A2 set as input on port pin
    AD1PCFGbits.PCFG2 = 0;   	// Set input to Analog
    IEC1bits.CMIE = 0;          // IE Off so no interrupt occurs from setup
    CM2CONbits.COE = 0;         // Disable output pin
    CM2CONbits.CPOL = 0;        // Standard sense. +In High ==> Out High
    CM2CONbits.EVPOL = 2;       // Event detected on output edge falling
    CM2CONbits.CREF = 1;        // +IN is internal CVRef
    CM2CONbits.CCH = 0b10;      // -IN is C1IND Pin
    
    CVRCON = 0x88;              // CVRef = (1/2) * (AVdd - AVss)
    
    CM2CONbits.CEVT = 0;        // Clear comparator event bit
    IFS1bits.CMIF = 0;          // Clear IF after set-up
       
    IEC1bits.CMIE = 1; 
    
}

void TimerInit(void) {
    
    T1CON = 0x00;               // Stops timer and resets control register settings
    T1CONbits.TSIDL = 0;        // Continue operation in idle mode
    T1CONbits.TCKPS = 0b10;     // 1:64 prescaler
    
    TMR1 = 0x00;                // Clear contents of the timer register
    PR1 = 63015;                // Count to 1 second (supposed to be 62500, but needed to calibrate it to get a correct frequency reading)
    
    IPC0bits.T1IP = 7;          // Set priority level of interrupt to 7 (highest priority)
    IEC0bits.T1IE = 1;          // Enable timer interrupts to occur
    IFS0bits.T1IF = 0;          // Clear timer 2 flag
    
}

int measureFrequency(void) {
    TimerInit();
    ComparatorInit();
    eventCount = 0;
    CM1CONbits.CON = 1; 	// Turn Comparator ON
    T1CONbits.TON=1;

} 
    timerActive = 0;

    while(!timerActive){};

    return eventCount;
}

void comparatorGo(void) {

    if (CM1CONbits.CEVT == 1) {
        eventCount++; 		// Count edges for whoever uses them
        CM1CONbits.CEVT = 0;
    }
}

void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void) {
    
    comparatorGo();
    
	IFS1bits.CMIF = 0;		// clear IF flag
        
    return;
    
}
