//Include the compiler structures
#include <xc.h>



void Delay_ms(int time_ms); //Function prototype of Delay_ms()

void Delay_ms(int time_ms){
    /*
     * Implements the delay functions used to time the LED blinks.
     * time_ms is the user specified time delay in milliseconds.
     */
    
    T2CONbits.TSIDL = 0; //Operate while in idle mode
    T2CONbits.T32 = 0; //Use timer 2 as a 16-bit timer
    T2CONbits.TCS = 0; //Use the internal clock of microprocessor
    
    //Set the input clock frequency to 8 MHz
    OSCCONbits.COSC0 = 1;
    OSCCONbits.COSC1 = 1;
    OSCCONbits.COSC2 = 1;
    
    //Set the pre-scaler to multiply the (2/8 MHz) or (0.25 microseconds) input clock period by 256
    T2CONbits.TCKPS0 = 1;
    T2CONbits.TCKPS1 = 1;
    
    IPC1bits.T2IP = 3;  //Set priority level of interrupt to 7 (highest priority)
    IEC0bits.T2IE = 1;  //Enable timer interrupts to occur
    IFS0bits.T2IF = 0;  //Clear timer 2 flag
    
    
    
    /* Note to self:  TAKE OUT COMMENT BEFORE SUBMITTING!!!
     * 
     * Quick Note: When setting TCS = 0, since we use internal clock which
     * has (Fosc)/2 that's why the PR2 values are halved from what we calculated 
     * on Wednesday. I'm not 100% sure on this but i do think this is the most probable reason
     */
    //(8MHz/2 = 4000000 clock ticks per second)
    // 4000000/256 = 15,625 clock ticks per second after prescaling
    // 1/4000000 = 25 microseconds per clock tick
    
   //PR2 = (time_ms)/(1000 * 0.00000025 *2* 256); //Calculates the value required for PR2 according to the argument timer_ms provided
   //PR2 = time_ms/0.064;
    PR2 = time_ms*15.625;

    //PR2 = (time_ms/1000)*15625;
    TMR2 = 0; //Clear the TMR2 register before running timer2
    
    T2CONbits.TON = 1; //Start timer2
    
    Idle(); //Put the microcontroller to idle mode
    
    TMR2 = 0; //Clear timer2 before exiting

}
