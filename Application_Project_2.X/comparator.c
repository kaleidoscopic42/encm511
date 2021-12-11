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
    CM1CONbits.EVPOL = 0b01; 	// Event detected on output edge falling
    CM1CONbits.CREF = 1; 	// +IN is internal CVRef
    CM1CONbits.CCH = 0b01; 	// -IN is C1INC Pin
    
    CVRCON = 0x0088; 		// CVRef = (1/2) * (AVdd - AVss)
    
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

void measureCapacitance(void) {
    
    TimerInit();
    ComparatorInit();
    
    TRISBbits.TRISB1 = 0;   // Configure as output
    PORTBbits.RB1 = 0;      // Set RB1 to low to allow capacitor to discharge
    Delay_ms(1000);         // Wait one second to allow capacitor to fully discharge
    
    TRISBbits.TRISB1 = 1;   // Configure as input
    TRISBbits.TRISB8 = 0;   // Configure as ouput
    
    CM1CONbits.CON = 1; 	// Turn Comparator ON
    T1CONbits.TON=1;        // Turn timer ON
    PORTBbits.RB8=1;        // Turn RB8 high
    
    capacitanceReady = 0;
    
    while(!capacitanceReady){};
    
    int halfTime = TMR1;
    
    Disp2String("Capacitance Measurement: ");
    float capacitance_conversion = halfTime/68.0;
    if(capacitance_conversion < 0) capacitance_conversion *= -1.0;
    char str_capacitance[10];
    sprintf(str_capacitance, "%.2f", capacitance_conversion);
    Disp2String(str_capacitance);
    Disp2String(" uF");
    
    XmitUART2('\n', 1);
    XmitUART2('\r', 1);
    
}


void comparatorGo(void) {

    if (CM1CONbits.CEVT == 1) {
        eventCount++; 		// Count edges for whoever uses them
        capacitanceReady = 1;
        CM1CONbits.CEVT = 0;
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
