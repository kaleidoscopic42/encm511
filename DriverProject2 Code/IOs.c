#include <xc.h>

void IOinit(); //Function prototype of IOinit()
void IOcheck(); //Function prototype of IOcheck()
void Delay_ms(int time_ms); //Function prototype of Delay_ms();

void IOinit(){ //This Function is responsible for initializing the appropriate IO ports
    
    TRISAbits.TRISA4 = 1; //Sets RA4 as an input (Pushbutton1 input)
    TRISBbits.TRISB4 = 1; //Sets RB4 as an input (Pushbutton2 input)
    TRISBbits.TRISB7 = 1; //Sets RB7 as an input (Pushbutton3 input)
    TRISBbits.TRISB8 = 0; //Set B8 as an output (LED output)
    CNPU1bits.CN0PUE = 1; // Pull CN0 - RA4 Up (PU pushbutton1 active low)
    CNPU1bits.CN1PUE = 1; // Pull CN1 - RB4 Up (PU pushbutton2 active low)
    CNPU2bits.CN23PUE = 1; // Pull CN23 - RB7 Up (PU pushbutton3 active low)
}

void IOcheck(){  
    /*
     * Implements the IO checks and LED blinking functionality, i.e. it determines
     * the length of the interval at which the LED blinks based on which pushbutton 
     * is pressed
     */
    
      if ((PORTAbits.RA4==0)){  //Pushbutton1 pressed
          
          /*
           * The if-else/else-if statements below check if a second pushbutton in 
           * addition to pushbutton1 is being pressed
           */
          
          if((PORTBbits.RB4==0)){ 
              /*
               * If pushbutton2 is also pressed down along with pushbutton1
               * then the LED will stay on without blinking until both or 
               * either pushbutton1 or pushbutton2 is no longer being pressed
               */
               LATBbits.LATB8 = 1; //Turn LED on
          }
          else if((PORTBbits.RB7==0)){
              /*
               * If pushbutton3 is also pressed down along with pushbutton1
               * then the LED will stay on without blinking until both or 
               * either pushbutton1 or pushbutton3 is no longer being pressed
               */
               LATBbits.LATB8 = 1;  //Turn LED on
          } 
          
          else{
              /*
               * If only pushbutton1 is being pressed, then the LED blinks
               * at an interval of 1 second (1 second one and 1 second off)
               */
            LATBbits.LATB8 = 1;  //Turn LED on          
            Delay_ms(1000);  //Keep LED on for 1000 milliseconds or 1 second
            LATBbits.LATB8 = 0; //Turn LED off
            Delay_ms(1000); //Keep LED off for 1000 milliseconds or 1 second
          }
        }
      else if((PORTBbits.RB4==0)){ //Pushbutton2 pressed
          
           /*
           * The if-else/else-if statements below check if a second pushbutton in 
           * addition to pushbutton2 is being pressed
           */
          
          if((PORTAbits.RA4==0)){
              /*
               * If pushbutton1 is also pressed down along with pushbutton2
               * then the LED will stay on without blinking until both or 
               * either pushbutton1 or pushbutton2 is no longer being pressed
               */
               LATBbits.LATB8 = 1;  //Turn LED on
          }
          else if((PORTBbits.RB7==0)){
              /*
               * If pushbutton3 is also pressed down along with pushbutton2
               * then the LED will stay on without blinking until both or 
               * either pushbutton2 or pushbutton3 is no longer being pressed
               */
               LATBbits.LATB8 = 1; //Turn LED on
          } else{
              /*
               * If only pushbutton2 is being pressed, then the LED blinks
               * at an interval of 2 second (2 second one and 2 second off)
               */
            LATBbits.LATB8 = 1;//Turn LED on       
            Delay_ms(2000); //Keep LED on for 2000 milliseconds or 2 second
            LATBbits.LATB8 = 0;//Turn LED off
            Delay_ms(2000); //Keep LED off for 2000 milliseconds or 2 second
          }
        }
      else if((PORTBbits.RB7==0)){ //Pushbutton3 pressed
          
           /*
           * The if-else/else-if statements below check if a second pushbutton in 
           * addition to pushbutton3 is being pressed
           */
          
          if((PORTAbits.RA4==0)){
              /*
               * If pushbutton1 is also pressed down along with pushbutton3
               * then the LED will stay on without blinking until both or 
               * either pushbutton3 or pushbutton1 is no longer being pressed
               */
               LATBbits.LATB8 = 1; //Turn LED on
          }
          else if((PORTBbits.RB4==0)){
              /*
               * If pushbutton2 is also pressed down along with pushbutton3
               * then the LED will stay on without blinking until both or 
               * either pushbutton2 or pushbutton3 is no longer being pressed
               */
               LATBbits.LATB8 = 1; //Turn LED on
          } else{
              /*
               * If only pushbutton1 is being pressed, then the LED blinks
               * at an interval of 3 second (3 second one and 3 second off)
               */
            LATBbits.LATB8 = 1; //Turn LED on       
            Delay_ms(3000); //Keep LED on for 3000 milliseconds or 3 second
            LATBbits.LATB8 = 0; //Turn LED off
            Delay_ms(3000); //Keep LED off for 3000 milliseconds or 3 second
          }
      } 
      
      else if(((PORTAbits.RA4==0) && (PORTBbits.RB4==0) && (PORTAbits.RA3==0))){ 
              /*
               * If all pushbuttons are being pressed simultaneously,
               * then the LED will stay on without blinking until all pushbuttons or 
               * two of the pushbuttons are no longer being pressed 
               */
          LATBbits.LATB8 = 1;   //Turn on LED
      } 
       
      else{
          //LED remains off if no buttons are pressed
            LATBbits.LATB8 = 0;         
      }
}
