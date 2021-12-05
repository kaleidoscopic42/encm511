#include "xc.h"
//THIS CODE WILL NOT COMPILE UNLESS YOU ADD YOUR TIME DELAYS AND RELEVANT PINS
    int i;
    int j = 0; //index for loop
    unsigned int Vread = 0;
    unsigned int Vavg = 0;
    unsigned int VTot = 0;
    //assume CTMU and A/D have been setup correctly
    //see Example 11-1 for CTMU & A/D setup
    setup();
    CTMUCONbits.CTMUEN = 1;//Enable the CTMU
    
    while (1){
    for([How ever many iterations you want])
    {
        AD1CON1bits.SAMP = 1;  //Manual sampling start - move to after IDISSEN=0 if long time delay
        CTMUCONbits.IDISSEN = 1; //drain charge on the circuit
       

        [   TIME DELAY HERE   ]
        
        CTMUCONbits.IDISSEN = 0; //end drain of circuit
        
        CTMUCONbits.EDG1STAT = 1; //Begin charging the circuit
        //using CTMU current source
       

        [     TIME DELAY HERE    ]
        
        CTMUCONbits.EDG1STAT = 0; //Stop charging circuit
        IFS0bits.AD1IF = 0; //make sure A/D Int not set
        AD1CON1bits.SAMP = 0; //and begin A/D conv.
        while(!AD1CON1bits.DONE); //Wait for A/D convert complete
        Vread = ADC1BUF0; //Get the value from the A/D
        IFS0bits.AD1IF = 0; //Clear A/D Interrupt Flag
        VTot += Vread; //Add the reading to the total
       
    }
    //If you are using multiple loops, don't forget to divide to average
    Vavg = (VTot); //1 Measurement
    
    Disp2Dec(Vavg);
    XmitUART2('\n', 1);
    XmitUART2('\r', 1);
    VTot = 0;
   
 }

And this is the initialization code:

void setup(void)
    { //CTMUCON - CTMU Control register
     
    CTMUCON = 0x0000; //make sure CTMU is disabled
    CTMUCONbits.CTMUEN = 0;
    CTMUCONbits.TGEN = 1;
    CTMUCONbits.EDG1POL = 1;
    CTMUCONbits.EDG2POL = 1;
    
    // CTMU continues to run when emulator is stopped,CTMU continues
    // to run in Idle mode, Time Generation mode enabled, Edges are
    // blocked. No edge sequence order, Analog current source not
    // grounded, trigger output disabled, Edge2 polarity = positive level,
    // Edge2 source = source 0, Edge1 polarity = positive level,
    // Edge1 source = source 0, Set Edge status bits to zero
    //CTMUICON - CTMU Current Control Register
    CTMUICON = 0x0000; 
    CTMUICONbits.IRNG0 = 1; //.55uA, Nominal - No Adjustment
    CTMUICONbits.IRNG1 = 0; //.55uA, Nominal - No Adjustment
    /**************************************************************************/
    /* Set up AD Converter ****************************************************/
    /**************************************************************************/
    
    
    AD1CON1bits.ADON=0;            //Turn off ADC, turn on before sampling in do_ADC()
    AD1CON1bits.ADSIDL=0;        //Continue ADC operation in idle mode
    AD1CON1bits.FORM=0b00;        //0b00: Int, 01: Signed Int, 10:Fractional, 11:Signed Fract
    AD1CON1bits.SSRC=0b000;    //Start ADC conversion Time set by SAMC has passed
    AD1CON1bits.ASAM=0;     //Next Sampling begins when SAMP bit is set
    AD1CON1bits.SAMP=0;        //Set this bit in ADC routine to start sampling  

    // Sampling time and ADC Clk select
    AD1CON2bits.VCFG=0b000;     //Vref+=AVDD   Vref-=AVSS
    AD1CON2bits.CSCNA=0;    //Do not scan inputs; use channel selected by CH0SA bits
    AD1CON2bits.SMPI=0b0000; //Any ADC interrrups occur at completion of each sample/convert seq.
    AD1CON2bits.BUFM=0; //Buffer confugured as 1 16 word buffer 
    AD1CON2bits.ALTS=0; //Always use MUXA input MUX settings
    AD1CON3bits.ADRC=0; //0: Use System clk; 1: ADC uses internal RC clock for idle or sleep mode
    AD1CON3bits.SAMC=0; 
    AD1CON3bits.ADCS=0; //ADCS conversion clock select bits. Ignored if using internal AD RC clock. 
    
    // MUX settings
    AD1CHSbits.CH0NB=0;    //Ch0 MUXB -ve Input is VR-
    AD1CHSbits.CH0SB=0bXXXX;    //YOUR ADC CHANNEL HERE
    AD1CHSbits.CH0NA=0;    //Ch0 -ve Input is VR-
    AD1CHSbits.CH0SA=0bXXXX;    //YOUR ADC CHANNEL HERE
   
    //IO port selection
    TRISAbits.TRISNNN = N;  //SET YOUR PIN AS AN INPUT
    AD1PCFGbits.PCFGN=N;     //SET YOUR PIN AS AN ANALOG INPUT
    AD1CON1bits.ADON=1;
    
    // Unsigned fractional format, Clear SAMP bit to
    // start conversion, Sample when SAMP bit is set,
    // sampling on hold
    
}


/*
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
*/

