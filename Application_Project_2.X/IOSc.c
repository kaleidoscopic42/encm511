#include <xc.h>
#include "ADC.h"
#define NO_BUTTON_PRESSED  (0b00000000)  // Case of no PB pressed
#define BUTTON1_PRESSED    (0b00000001) // Case of PB1 pressed corresponds to 1 in bit 0
#define BUTTON2_PRESSED    (0b00000010) // Case of PB2 pressed corresponds to 1 in bit 1
#define BUTTON3_PRESSED    (0b00000100) // Case of PB3 pressed corresponds to 1 in bit 2

uint16_t buttons_pressed = NO_BUTTON_PRESSED;

uint16_t CN0flag = NO_BUTTON_PRESSED;
uint16_t CN1flag = NO_BUTTON_PRESSED;
uint16_t CN30flag = NO_BUTTON_PRESSED;

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
      case (BUTTON1_PRESSED):   // Case of PB1 being pressed
          get_AN5_measurement();
        break;
      case(BUTTON2_PRESSED):    // Case of PB2 being pressed
          get_AN11_measurement();
        break;
      case(BUTTON3_PRESSED):    // Case of PB3 being pressed
          Disp2String("Capacitance: ");
          Disp2Dec(measureCapacitance());
          Disp2String(" uF");
        break;
      default:;
          int frequency = measureFrequency();
          if(!frequency) {
              Idle();
          }
          else {
              Disp2String("Frequency: ");
              Disp2Dec(frequency);
              Disp2String(" Hz");
          }
        // Add code here to put in doze mode when there is no frequency source
    }
}

    void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt (void) // ISR triggered by buttons
{

    if(check_active()) {return;}

    IFS1bits.CNIF = 0; // clear IF flag

    int buttonA2 = PORTAbits.RA2; // Read state of PB1
    int buttonRB4 = PORTBbits.RB4; // Read state of PB2
    int buttonRA4 = PORTAbits.RA4; // Read state of PB3

    Delay_ms(2); // Debounce - wait for steady state

    if(PORTAbits.RA2 == buttonA2) { // If current state of PB1 is same as previous state

        if (PORTAbits.RA2 == buttonA2 && PORTAbits.RA2 == 0){ // If PB1 is pressed
            CN30flag = BUTTON1_PRESSED; // user defined global variable used as flag
        }
        else {
            CN30flag = NO_BUTTON_PRESSED;    // If PB1 is not pressed, set flag to 0
        }
    }
    if(PORTBbits.RB4 == buttonRB4 ) { // If current state of PB2 is same as previous state

        if( PORTBbits.RB4 == buttonRB4 && PORTBbits.RB4 == 0) // If PB2 is pressed
        {
            CN1flag = BUTTON2_PRESSED; // user defined global variable used as flag
        }
        else {
            CN1flag = NO_BUTTON_PRESSED;         // If PB2 is not pressed, set flag to 0
        }
    }
    if(PORTAbits.RA4 == buttonRA4) { // If current state of PB3 is same as previous state
        if( PORTAbits.RA4 == buttonRA4 && PORTAbits.RA4 == 0) // If PB3 is pressed
        {
            CN0flag = BUTTON3_PRESSED; // user defined global variable used as flag
        }
        else {
            CN0flag = NO_BUTTON_PRESSED;       // If PB3 is not pressed, set flag to 0

        }
    }
   buttons_pressed = CN0flag | CN1flag | CN30flag; // State variable is OR of flags for every button
  /*  if(PORTAbits.RA2 == buttonA2 && PORTAbits.RA2 == 0) { // If current state of PB1 is same as previous state

        get_AN5_measurement();

    }

    if(PORTBbits.RB4 == buttonRB4 && PORTBbits.RB4 == 0) { // If current state of PB2 is same as previous state

        get_AN11_measurement();

    }*/

    Nop(); // No operation

    return; // Return
}
