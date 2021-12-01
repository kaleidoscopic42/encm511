#include <xc.h>
#include "ADC.h"

void Delay_ms(int time_ms); //Function prototype of Delay_ms();

void IOinit(){ //This Function is responsible for initializing the appropriate IO ports

    TRISAbits.TRISA2 = 1;   // Sets RA2 as an input (Pushbutton1 input)
    CNPU2bits.CN30PUE = 1;   // Pull CN30 - RA2 Up (PU pushbutton1 active low)
    
    TRISBbits.TRISB4 = 1;   // Sets RB4 as an input (Pushbutton1 input)
    CNPU1bits.CN1PUE = 1;   // Pull CN1 - RB4 Up (PU pushbutton1 active low)
    
    TRISAbits.TRISA4 = 1;   // Sets RA4 as an input (Pushbutton1 input)
    CNPU1bits.CN0PUE = 1;   // Pull CN0 - RA4 Up (PU pushbutton1 active low)
    
    CNEN2bits.CN30IE = 1;    // Enable CN30 interrupt
    CNEN1bits.CN1IE = 1;    // Enable CN1 interrupt
    CNEN1bits.CN0IE = 1;    // Enable CN0 interrupt

    IPC4bits.CNIP = 7;      // Set CN interrupt priority
    IFS1bits.CNIF = 0;      // Clears the CN interrupt flag
    IEC1bits.CNIE = 1;      // Enable the CN interrupt (general)

}

void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt (void) // ISR triggered by buttons 
{
    
    if(check_active()) {return;}
    
    IFS1bits.CNIF = 0; // clear IF flag
    
    int buttonA2 = PORTAbits.RA2; // Read state of PB1
    int buttonRB4 = PORTBbits.RB4; // Read state of PB2
    int buttonRA4 = PORTAbits.RA4; // Read state of PB3
    
    Delay_ms(2); // Debounce - wait for steady state
    
    if(PORTAbits.RA2 == buttonA2 && PORTAbits.RA2 == 0) { // If current state of PB1 is same as previous state
        
        get_AN5_measurement();
        
    }
    
    if(PORTBbits.RB4 == buttonRB4 && PORTBbits.RB4 == 0) { // If current state of PB2 is same as previous state
        
        get_AN11_measurement();
        
    }

    Nop(); // No operation

    return; // Return
}
