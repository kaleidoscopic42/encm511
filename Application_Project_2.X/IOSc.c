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

void IOcheck(){                 // This function is responsible for checking which buttons are pressed, controlling the LED and displaying messages

    switch(buttons_pressed)     // Look at which buttons are pressed
    {
      case (VOLT):   // If PB1 is being pressed, voltmeter mode
          get_AN5_measurement();    // Get voltage measurement from analog input AN5 and display
        break;
      case(OHM):    // If PB2 is being pressed, ohmmeter mode
          get_AN11_measurement(); // Get measurement from analog input AN11, convert to ohms and display
        break;
      case(CAP):    // If PB3 is being pressed, capacitor mode
          measureCapacitance()); // Use comparator to find capacitance
        break;
      case(FREQ): // If no buttons are pressed, frequency mode
        int frequency = measureFrequency(); // Use comparator to find frequency
          if(!frequency) {      // If frequency is zero (no frequency source)
              Idle();           // Idle() for power saving
     }
}

    void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt (void) // ISR triggered by buttons
{

    if(check_active()) {return;}

    IFS1bits.CNIF = 0; // clear IF flag
            
    
    Delay_ms(10); // Debounce - wait for steady state

    buttonA2  = PORTAbits.RA2; // Read state of PB1
    buttonRB4 = PORTBbits.RB4; // Read state of PB2
    buttonRA4 = PORTAbits.RA4; // Read state of PB3

        if (!buttonA2 && buttonRB4 && buttonRA4){ // If PB1 is pressed
            mode = VOLT;        // Voltmeter mode
        }
        else if (buttonA2 && !buttonRB4 && buttonRA4) { // If PB2 is pressed
            mode = OHM;         // Ohmmeter mode
        }
        else if(buttonA2 && buttonRB4 && !buttonRA4) { // If PB3 is pressed
            mode = CAP;         // Capacitor mode
        }
        else {                  // Other cases (no buttons pressed)
            mode = FREQ;        // Frequency mode
        }

    Nop(); // No operation

    return; // Return
}
