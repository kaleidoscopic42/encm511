#include "xc.h"

int timerActive = 0;
int capacitanceReady = 0;
int eventCount = 0;

void ComparatorInit(void) {
    
    IEC1bits.CMIE = 0;      // Disable interrupts while setting up pin
    
    TRISBbits.TRISB1 = 1; 	// A3 set as input on port pin
    AD1PCFGbits.PCFG3 = 0; 	// Set input to Analog
    CM1CONbits.COE = 0; 	// Disable output pin
    CM1CONbits.CPOL = 0; 	// Standard sense. +In High ==> Out High
    CM1CONbits.EVPOL = 2; 	// Event detected on output edge falling
    CM1CONbits.CREF = 1; 	// +IN is internal CVRef
    CM1CONbits.CCH = 0b01; 	// -IN is C1INC Pin
    
    TRISAbits.TRISA2 = 1;       // A2 set as input on port pin
    AD1PCFGbits.PCFG4 = 0;   	// Set input to Analog
    CM2CONbits.COE = 0;         // Disable output pin
    CM2CONbits.CPOL = 0;        // Standard sense. +In High ==> Out High
    CM2CONbits.EVPOL = 2;       // Event detected on output edge falling
    CM2CONbits.CREF = 1;        // +IN is internal CVRef
    CM2CONbits.CCH = 0b10;      // -IN is C2IND Pin
    
    CM2CONbits.CEVT = 0;        // Clear comparator event bit
    
    CVRCON = 0x88; 		// CVRef = (1/2) * (AVdd - AVss)
    
    CM1CONbits.CEVT = 0;    // Clear comparator event bit
    IFS1bits.CMIF = 0; 		// Clear IF after set-up
       
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
    CM1CONbits.CON = 1; 	// Turn Comparator ON
    T1CONbits.TON=1;
    
    timerActive = 0;
    
    while(!timerActive){};
    
    return eventCount;
}

int measureCapacitance(void) {
    
    Disp2String("Hello6");
    TimerInit();
    ComparatorInit();
    
    Disp2String("Hello5");
    
    TRISAbits.TRISA2 = 0;   // Configure as 
    Disp2String("Hello51");
    PORTAbits.RA2 = 0;      // Set RA2 to low to allow capacitor to discharge
    Disp2String("Hello52");
    Delay_ms(1000);         // Wait one second to allow capacitor to fully discharge
    Disp2String("Hello53");
    
    Disp2String("Hello4");
    
    TRISAbits.TRISA2 = 1;   // Configure as input
    TRISBbits.TRISB8 = 0;   // Configure as ouput
    
    Disp2String("Hello3");
    
    CM2CONbits.CON = 1; 	// Turn Comparator ON
    T1CONbits.TON=1;        // Turn timer ON
    PORTBbits.RB8=1;        // Turn RB8 high (charge capacitor)
    
    Disp2String("Hello2");
    
    capacitanceReady = 0;
    
    while(!capacitanceReady){};
    
    Disp2String("Hello1");
    
    int halfTime = TMR2;
    
    return halfTime/(100*0.69315);
    
}


void comparatorGo(void) {

    if (CM1CONbits.CEVT == 1) {
        eventCount++; 		// Count edges for whoever uses them
        CM1CONbits.CEVT = 0;
    }
    
    if (CM2CONbits.CEVT == 1) {
        capacitanceReady = 1;
        CM2CONbits.CEVT = 0;
    }
}

void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void) {
    
    comparatorGo();
    
	IFS1bits.CMIF = 0;		// clear IF flag
        
    return;
    
}

void __attribute__((__interrupt__, __shadow__)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0;     //Reset Timer1 interrupt flag and Return from ISR}
    timerActive = 1;
}
