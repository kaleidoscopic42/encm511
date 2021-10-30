#include <xc.h>

#define BUTTON1_PRESSED    (0b00000001)
#define BUTTON2_PRESSED    (0b00000010)
#define BUTTON3_PRESSED    (0b00000100)
#define BUTTON1_2PRESSED   (0b00000011)
#define BUTTON1_3PRESSED   (0b00000101)
#define BUTTON2_3PRESSED   (0b00000110)
#define BUTTON1_2_3PRESSED (0b00000111)

uint8_t  CN1flag =  0b00000000;
uint8_t  CN0flag =  0b00000000;
uint8_t  CN23flag = 0b00000000;

 uint8_t buttons_pressed = 0b00000000;


void IOinit(); //Function prototype of IOinit()
void IOcheck(); //Function prototype of IOcheck()
void Delay_ms(int time_ms); //Function prototype of Delay_ms();
void blink_LED();


void blink_LED(int ms) {

  LATBbits.LATB8 = 1; // Turn LED on
  Delay_ms(ms);                 //clear the global flag variable for PB
  LATBbits.LATB8 = 0;
  Delay_ms(ms);
}

void IOinit(){ //This Function is responsible for initializing the appropriate IO ports

    TRISAbits.TRISA4 = 1; //Sets RA4 as an input (Pushbutton1 input)
    TRISBbits.TRISB4 = 1; //Sets RB4 as an input (Pushbutton2 input)
    TRISBbits.TRISB7 = 1; //Sets RB7 as an input (Pushbutton3 input)
    TRISBbits.TRISB8 = 0; //Set B8 as an output (LED output)
    CNPU1bits.CN0PUE = 1; // Pull CN0 - RA4 Up (PU pushbutton1 active low)
    CNPU1bits.CN1PUE = 1; // Pull CN1 - RB4 Up (PU pushbutton2 active low)
    CNPU2bits.CN23PUE = 1; // Pull CN23 - RB7 Up (PU pushbutton3 active low)

    CNEN1bits.CN0IE = 1;    //Enable CN0 interrupt
    CNEN1bits.CN1IE = 1;    //enable CN1 interrupt
    CNEN2bits.CN23IE = 1;    //Enable CN0 interrupt
    IPC4bits.CNIP = 4;      //set CN interrupt priority
    IFS1bits.CNIF = 1;      //clears the CN interrupt flag
    IEC1bits.CNIE = 1;      //enable the CN interrupt (general)

}

void IOcheck(){

  //uint8_t buttons_pressed = 0b00000000;

  int period1 = 500; // Period in ms
  int period2 = 2000;
  int period3 = 3000;


    switch(buttons_pressed)
    {
      case (BUTTON1_PRESSED):
        blink_LED(period1);
        break;
      case(BUTTON2_PRESSED):
        blink_LED(period2);
        break;
      case(BUTTON3_PRESSED):
        blink_LED(period3);
        break;
      case(BUTTON1_2PRESSED):
      case(BUTTON1_3PRESSED):
      case(BUTTON2_3PRESSED):
      case(BUTTON1_2_3PRESSED):
        LATBbits.LATB8 = 1;
        break;
      default:
        LATBbits.LATB8 = 0;
    }
}

void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt (void)
{
    if(IFS1bits.CNIF == 1)
    {
        if (PORTAbits.RA4 == 0){
            CN0flag = 0b00000001; // user defined global variable used as flag
        }
        else{
            CN0flag = 0;
        }
        if(PORTBbits.RB4 ==0)
        {
            CN1flag = 0b00000010; // user defined global variable used as flag
        }
        else {
            CN1flag = 0;
        }

        if(PORTBbits.RB7 == 0)
        {
            CN23flag = 0b00000100;

        }else {
            CN23flag = 0;
        }

    }
   buttons_pressed = CN0flag | CN1flag | CN23flag;

    IFS1bits.CNIF = 0; // clear IF flag

    Nop();

    return;
}
