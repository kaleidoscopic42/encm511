#include <xc.h>

// Binary values named based on buttons to be used for state machine

#define NO_BUTTON_PRESSED (0b00000000)  // Case of no PB pressed
#define BUTTON1_PRESSED    (0b00000001) // Case of PB1 pressed corresponds to 1 in bit 0
#define BUTTON2_PRESSED    (0b00000010) // Case of PB2 pressed corresponds to 1 in bit 1
#define BUTTON3_PRESSED    (0b00000100) // Case of PB3 pressed corresponds to 1 in bit 2
#define BUTTON1_2PRESSED   (0b00000011) // Case of PB1 and PB2 pressed corresponds to BUTTON1_PRESSED OR BUTTON_2PRESSED
#define BUTTON1_3PRESSED   (0b00000101) // Case of PB1 and PB3 pressed corresponds to BUTTON1_PRESSED OR BUTTON_3PRESSED
#define BUTTON2_3PRESSED   (0b00000110) // Case of PB2 and PB2 pressed corresponds to BUTTON2_PRESSED OR BUTTON_3PRESSED
#define BUTTON1_2_3PRESSED (0b00000111) // Case of all PBs pressed correspondsto BUTTON1_PRESSED OR BUTTON_2PRESSED OR BUTTON3_PRESSED

uint8_t  CN1flag =  NO_BUTTON_PRESSED; // Flag indicating that CN interrupt was triggered by PB1
uint8_t  CN0flag =  NO_BUTTON_PRESSED; // Flag indicating that CN interrupt was triggered by PB2
uint8_t  CN23flag = NO_BUTTON_PRESSED; // Flag indicating that CN interrupt was triggered by PB3

uint8_t buttons_pressed = NO_BUTTON_PRESSED; // State variable to contain combination of all button presses

int button1 = 0; // Variables used to debounce each PB
int button2 = 0;
int button3 = 0;


 const int period1 = 500;  // Blink period for PB1 in ms
 const int period2 = 2000; // Blink period for PB2 in ms
 const int period3 = 3000; // Blink period for PB3 in ms
 int period = 0;           // Variable period 
 
 int LED_state = 0;        // Variable will determine if LED stays off or on
 
 const char button_message0[] =     "Nothing pressed          \r"; // Message for no PBs pressed
 const char button_message1[] =     "PB1 is pressed           \r"; // Message for PB1 pressed
 const char button_message2[] =     "PB2 is pressed           \r"; // Message for PB2 pressed
 const char button_message3[] =     "PB3 is pressed           \r"; // Message for PB3 pressed
 const char button_message1_2[] =   "PB1 and PB2 are pressed  \r"; // Message for PB1 and PB2 pressed
 const char button_message1_3[] =   "PB1 and PB3 are pressed  \r"; // Message for PB1 and PB3 pressed
 const char button_message2_3[] =   "PB2 and PB3 are pressed  \r"; // Message for PB2 and PB3 pressed
 const char button_message1_2_3[] = "All PBs pressed          \r"; // Message for all of PB1, PB2 and PB3 pressed
 
 char* button_message = button_message0; // Initial message is for no PBs pressed


void IOinit(); //Function prototype of IOinit()
void IOcheck(); //Function prototype of IOcheck()
void Delay_ms(int time_ms); //Function prototype of Delay_ms();
void blink_LED();   // Function prototype of blink_LED()


void blink_LED(int ms) { // A function that takes a period (in ms) as an argument and blinks the LED at that period

  LATBbits.LATB8 = !LATBbits.LATB8; // Turn LED off or on depending on previous state
  Delay_ms(ms);                     // Wait for the amount of time specified by ms
 
}

void IOinit(){ //This Function is responsible for initializing the appropriate IO ports

    TRISAbits.TRISA4 = 1; // Sets RA4 as an input (Pushbutton1 input)
    TRISBbits.TRISB4 = 1; // Sets RB4 as an input (Pushbutton2 input)
    TRISBbits.TRISB7 = 1; // Sets RB7 as an input (Pushbutton3 input)
    TRISBbits.TRISB8 = 0; // Set RB8 as an output (LED output)
    CNPU1bits.CN0PUE = 1; // Pull CN0 - RA4 Up (PU pushbutton1 active low)
    CNPU1bits.CN1PUE = 1; // Pull CN1 - RB4 Up (PU pushbutton2 active low)
    CNPU2bits.CN23PUE = 1; // Pull CN23 - RB7 Up (PU pushbutton3 active low)

    CNEN1bits.CN0IE = 1;    //Enable CN0 interrupt
    CNEN1bits.CN1IE = 1;    //Enable CN1 interrupt
    CNEN2bits.CN23IE = 1;   //Enable CN0 interrupt
    IPC4bits.CNIP = 4;      //Set CN interrupt priority
    IFS1bits.CNIF = 1;      //Clears the CN interrupt flag
    IEC1bits.CNIE = 1;      //Enable the CN interrupt (general)

}

void IOcheck(){                 // This function is responsible for checking which buttons are pressed, controlling the LED and displaying messages
   
    switch(buttons_pressed)     // Look at which buttons are pressed
    {
      case (BUTTON1_PRESSED):   // Case of PB1 being pressed
          period = period1;     // Period becomes period assigned to PB1
          button_message = button_message1; // Message becomes message assigned to PB1
        break;
      case(BUTTON2_PRESSED):    // Case of PB2 being pressed
          period = period2;     // Period becomes period assigned to PB2
          button_message = button_message2; // Message becomes message assigned to PB2
        break;
      case(BUTTON3_PRESSED):    // Case of PB3 being pressed
          period = period3;     // Period becomes period assigned to PB3
          button_message = button_message3; // Message becomes message assigned to PB3
        break;
      case(BUTTON1_2PRESSED):   // Case of PB1 and PB2 being pressed
          period = 0;           // No blink period; LED will be set to stay on
          button_message = button_message1_2; // Message becomes message assigned to combination of PB1 and PB2
          LED_state = 1;        // LED state will be on
          break;
      case(BUTTON1_3PRESSED):   // Case of PB1 and PB3 being pressed
          period = 0;           // No blink period; LED will be set to stay on
          button_message = button_message1_3; // Message becomes message assigned to combination of PB1 and PB3
          LED_state = 1;        // LED state will be on
          break;
      case(BUTTON2_3PRESSED):   // Case of PB2 and PB3 being pressed
          period = 0;           // No blink period; LED will be set to stay on
          button_message = button_message2_3; // Message becomes message assigned to combination of PB2 and PB3
          LED_state = 1;        // LED state will be on
          break;
      case(BUTTON1_2_3PRESSED): // Case of all PBs being pressed
          period = 0;           // No blink period; LED will be set to stay on
          button_message = button_message1_2_3; // Message becomes message assigned to combination of all PBs
          LED_state = 1;        // LED state will be on
        break;
      default:                  // Default case of no buttons being pressed
          period = 0;           // No blink period; LED will be set to stay off 
          button_message = button_message0; // Message becomes message displayed for no PBs being pressed
          LED_state = 0;        // LED state will be off
    }
    
        Disp2String(button_message); // Display message in terminal 

        if(!period) {           // If period is zero
            LATBbits.LATB8 = LED_state; // Set LED to stay on or off depending on assigned state
         }
        else {
            blink_LED(period); // Blink LED at the specified period in ms
        }
    
}

void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt (void) // ISR triggered by buttons 
{
     IFS1bits.CNIF = 0; // clear IF flag
     
    button1 = PORTAbits.RA4; // Read state of PB1 
    button2 = PORTBbits.RB4; // Read state of PB2
    button3 = PORTBbits.RB7; // Read state of PB3
    
    Delay_ms(2); // Debounce - wait for steady state
    
    if(PORTAbits.RA4 == button1) { // If current state of PB1 is same as previous state
        
        if ((PORTAbits.RA4) == 0){ // If PB1 is pressed
            CN0flag = BUTTON1_PRESSED; // user defined global variable used as flag
        }
        else {
            CN0flag = NO_BUTTON_PRESSED;    // If PB1 is not pressed, set flag to 0
        }
    }
    if(PORTBbits.RB4 == button2) { // If current state of PB2 is same as previous state
        
        if( (PORTBbits.RB4) == 0) // If PB2 is pressed
        {
            CN1flag = BUTTON2_PRESSED; // user defined global variable used as flag
        }
        else {
            CN1flag = NO_BUTTON_PRESSED;         // If PB2 is not pressed, set flag to 0
        }
    }
    if(PORTBbits.RB7 == button3) { // If current state of PB3 is same as previous state
        
        if( (PORTBbits.RB7) == 0) // If PB3 is pressed
        {
            CN23flag = BUTTON3_PRESSED; // user defined global variable used as flag
        }
        else {
            CN23flag = NO_BUTTON_PRESSED;       // If PB3 is not pressed, set flag to 0
        }
    }
   buttons_pressed = CN0flag | CN1flag | CN23flag; // State variable is OR of flags for every button

    Nop(); // No operation

    return; // Return
}
