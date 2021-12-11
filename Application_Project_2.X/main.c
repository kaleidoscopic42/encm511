// 'C' source line config statements

// 'C' source line config statements

// FBS
#pragma config BWRP = OFF               // Table Write Protect Boot (Boot segment may be written)
#pragma config BSS = OFF                // Boot segment Protect (No boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Flash Code Protection bit (No protection)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (WDT prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected; windowed WDT disabled)
#pragma config FWDTEN = OFF              // Watchdog Timer Enable bit (WDT enabled)

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware; SBOREN bit disabled)
#pragma config PWRTEN = OFF              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Default location for SCL1/SDA1 pins)
#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (MCLR pin enabled; RA5 input pin disabled)

// FICD
#pragma config ICS = PGx2               // ICD Pin Placement Select bits (PGC2/PGD2 are used for programming and debugging the device)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses LPRC as reference clock)
#pragma config RTCOSC = SOSC            // RTCC Reference Clock Select bit (RTCC uses SOSC as reference clock)
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = OFF             // Deep Sleep Watchdog Timer Enable bit (DSWDT enabled)

// CLOCK CONTROL 
#pragma config IESO = OFF    // 2 Speed Startup disabled
#pragma config FNOSC = FRC  // Start up CLK = 8 MHz
#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled
#pragma config SOSCSEL = SOSCLP // Secondary oscillator for Low Power Operation
#pragma config POSCFREQ = MS  //Primary Oscillator/External clk freq betwn 100kHz and 8 MHz. Options: LS, MS, HS
#pragma config OSCIOFNC = ON  //CLKO output disabled on pin 8, use as IO. 
#pragma config POSCMOD = NONE  // Primary oscillator mode is disabled

/*
 * The block of code above, from line 1 to line 41, was taken from
 * the example code provided by Professor Parker. I want to give credit
 * to Professor Parker because he provided us with example code
 * when going over Clock interrupts and delays regarding Driver Project 2
 */

#include <xc.h>
#include "UART2.h"
#include "ADC.h"

void IOinit(); //Function prototype of IOinit()
void IOcheck(); //Function prototype of IOcheck()
void Delay_ms(int);

int main(void){
    
    // Clock inputs on REFO/RB15
    TRISBbits.TRISB15 = 0;      // Set RB15 as output for REFO
    REFOCONbits.ROEN = 0;       // Reference oscillator is disabled
    REFOCONbits.ROSSLP = 0;     // Reference oscillator is disabled in sleep
    REFOCONbits.ROSEL = 0;      // Output base CLK showing clock swithcing
    REFOCONbits.RODIV = 0b1111; // ~ 250Hz
    REFOCONbits.ROEN = 1;       // Reference oscillator is enabled
    
//    // Initialize the ADC module and the push button
//    ADCinit();
    IOinit();
//    
//    // Print message to show that program has started
//    Disp2String("Start");
//    XmitUART2('\n', 1);
//    XmitUART2('\r', 1);

    Disp2String("Start");
    // Infinite while loop to make program run infinitely
    while(1){
        measureCapacitance();
        IOcheck();
    }

    return 0;
}
