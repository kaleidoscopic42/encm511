#include "xc.h"
#define    CTMU_MODE_EDGE 0
#define    RANGE_0_550uA 1    // .550uA
#define    ADSCALE 1023       //for 10-bit ADC
#define    ADREF 3.3          //Vdd connected to ADC Vr+
#define    DELAY_LOOPS 150    // delay = 6 cycles * DELAY_LOOPS * (1/4 MHz)  Fosc = 8MHz
unsigned   int CtmuCapMeasureConfig(unsigned int mode, unsigned int range, signed int trim){
    unsigned int result, x;// Step 1 Configure the CTMU
    CTMUCON = 0x0000;                   // Disable CTMU
    CTMUCONbits.
    CTMUCONbits.TGEN = mode;            // Enable/Disable Time Generation mode
    CTMUCONbits.EDGEN = 0;              // Edges are disabled
    CTMUCONbits.IDISSEN = 1;            // Current source is grounded (discharge enabled)
    CTMUCONbits.ITRIM = trim;           // Set trim
    CTMUCONbits.CTTRIG = 0;             // Trigger output disabled
    CTMUCONbits.IRNG = (range & 3);     // Set range
    CTMUCON = 0;                        // Edges are disabled, edge controls ignored
    // Next line does not apply to all devices
    CTMUCONbits.IRNGH = (range>>2);     // set high bit of range
    CTMUCONbits.IRSTEN = 0;             // Current source reset disabled
    CTMUCONbits.DSCHS = 1;              // Discharge source enabled
    // Step 2 Configure the port 
    TRISBbits = TRISB | (1<<2);               // Set channel 2
    ANSBbits.ANSB14 = 1;                  // Make AN2 as analog   
    // Step 3 configure the ADC
    AD1CHSbits.CH0SA = 2;
    AD1CON1 = 0x8000;                     // Turn On A/D Converter,
    // Unsigned fractional format, Clear SAMP bit to
    // start conversion, Sample when SAMP bit is set
    AD1CON2 = 0x0000;                     // VR+ = AVDD, V- = AVSS, Don't scan,
    AD1CON3 = 0x0000;                     // ADC uses system clock
    AD1CON3bits.ADCS = 0;                 // conversion clock = 1xTcy
    AD1CON5 = 0x0000;                     // Auto-Scan disabled
    //  Step 4 - 6 Enable the current source and start sampling
    CTMUCON1Lbits.CTMUEN = 1;             // Enable the CTMU
    CTMUCON1Hbits.EDG1STAT = 1;           // Enable current source
    AD1CON1bits.SAMP = 1;                 // Manual sampling start
    // step 7  1500us delay to discharge sample capfor (x = 0; x < 5000; x++);           
    // ~6 cycles * 5000 // step 9 disable discharge
    CTMUCON1Lbits.IDISSEN = 0;            // Discharge disabled
    // step 10 delay to charge sample capfor (x = 0; x < DELAY_LOOPS; x++);    
    // 6 clocks per loop iteration
    // step 11 convert the sample
    AD1CON1bits.SAMP = 0;                 // Begin A/D conversion 
    while(AD1CON1bits.DONE == 0);         // Wait for A/D convert complete
    // Step 12 disable the CTMU
    CTMUCON1Hbits.EDG1STAT = 0;           // Disable current source
    IFS0bits.AD1IF = 0;                   // Clear ADC interrupt flag
    CTMUCON1Lbits.CTMUEN = 0;             // Disable the 
    CTMUresult = ADC1BUF0;return (result);                      // return accumulated result}
#defineITT 10                            // 10 iterationsint     
    
main(void){
    float cntsAvg, cntsTot = 0, c;  
    unsigned int x;                  // Step 10 perform measurement 10 times
    for(x = 0; x < ITT; x++) {     
        cntsTot += (float)(CtmuCapMeasureConfig(CTMU_MODE_EDGE, RANGE_0_550uA, 0));    // .550uA, no trim    
    }// Step 11 calculate the result
    cntsAvg = (cntsTot / ITT);            // Average of 10 reading
    sc = (.550e-6 * DELAY_LOOPS * 6 * (1 / 4e6)) / (3.3 * cntsAvg / ADSCALE);     // capacitance in Farads

    // step 12
    // user code to perform iteration for calibration
    while(1);
}
