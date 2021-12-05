#include "xc.h"

void setup(void) {
    
    /**************************************************************************/
    /* Set up CTMU ************************************************************/
    /**************************************************************************/
     
    // CTMU configuration settings
    CTMUCON = 0x0000;                   // Reset configuration to default values
    CTMUCONbits.CTMUEN = 0;             // Disable the CTMU
    CTMUCONbits.TGEN = 0;               // Configured for current generation
    CTMUCONbits.EDGEN = 0;              // Edges are disabled
    CTMUCONbits.IDISSEN = 0;            // Current source is grounded (discharge enabled)
    CTMUICONbits.ITRIM = 0b000000;      // No trim
    CTMUCONbits.CTTRIG = 0;             // Trigger output disabled
    CTMUCONbits.CTMUSIDL = 0;           // CTMU continues operation in idle mode
    
    // CTMU current configuration
    CTMUICON = 0x0000;                  // Rest configuration to default values
    CTMUICONbits.IRNG = 0b01;           // .55uA, Nominal - No Adjustment
    
    /**************************************************************************/
    /* Set up AD Converter ****************************************************/
    /**************************************************************************/
    
    // ADC configuration settings
    AD1CON1bits.ADON=0;                 // Turn off ADC, turn on before sampling in do_ADC()
    AD1CON1bits.ADSIDL=0;               // Continue ADC operation in idle mode
    AD1CON1bits.FORM=0b00;              // 0b00: Int, 01: Signed Int, 10:Fractional, 11:Signed Fract
    AD1CON1bits.SSRC=0b000;             // Start ADC conversion Time set by SAMC has passed
    AD1CON1bits.ASAM=0;                 // Next Sampling begins when SAMP bit is set
    AD1CON1bits.SAMP=0;                 // Set this bit in ADC routine to start sampling
    AD1CON1bits.DONE = 0;               // A/D conversion is not done

    // Sampling time and ADC Clk select
    AD1CON2bits.VCFG=0b000;             // Vref+=AVDD   Vref-=AVSS
    AD1CON2bits.CSCNA=0;                // Do not scan inputs; use channel selected by CH0SA bits
    AD1CON2bits.SMPI=0b0000;            // Any ADC interrrups occur at completion of each sample/convert seq.
    AD1CON2bits.BUFM=0;                 // Buffer confugured as 1 16 word buffer 
    AD1CON2bits.ALTS=0;                 // Always use MUXA input MUX settings
    AD1CON3bits.ADRC=0;                 // Use System clk
    AD1CON3bits.SAMC=0;                 // 0 Tad
    AD1CON3bits.ADCS=0;                 // ADCS conversion clock select bits. Ignored if using internal AD RC clock. 
    
    // MUX settings
    AD1CHSbits.CH0NB=0;                 // Ch0 MUXB -ve Input is VR-
    AD1CHSbits.CH0SB=0b01100;           // Positive input is AN12/RB12/pin15 for MUXB
    AD1CHSbits.CH0NA=0;                 // Ch0 -ve Input is VR-
    AD1CHSbits.CH0SA=0b01100;           // Positive input is AN12/RB12/pin15 for MUXA
   
    // IO port selection
    TRISBbits.TRISB12 = 1;              // AN12/RB12/pin15 set as input
    AD1PCFG = 0xEFFF;                   // AN12/RB12/pin15 set as analog input
    AD1CON1bits.ADON=1;                 // Turn the ADC module on
    
    // Unsigned fractional format, Clear SAMP bit to
    // start conversion, Sample when SAMP bit is set,
    // sampling on hold
    
}

void CTMUMeasurement(void) {

    int i, j;
    unsigned int Vread = 0;
    unsigned int Vavg = 0;
    unsigned int VTot = 0;
    float c;

    setup();                            // Setup the CTMU and ADC for capacitance measurements
    
    CTMUCONbits.CTMUEN = 1;             //  Enable the CTMU
    
    // Ten measurements are taken to be averaged
    for(i = 0; i < 10; ++i) {
        
        AD1CON1bits.SAMP = 1;           // Manual sampling start - move to after IDISSEN=0 if long time delay
        CTMUCONbits.IDISSEN = 1;        // Drain charge on the circuit

        // one iteration is ~458 clock cycles, so 6 loops discharges for ~1.4ms
        for(j = 0; j < 6; ++j);

        CTMUCONbits.IDISSEN = 0;        // End drain of circuit

        CTMUCONbits.EDG1STAT = 1;       // Begin charging the circuit

        // one iteration is ~458 clock cycles, so 1 loops discharges for ~229us
        for(j = 0; j < 1; ++j);

        CTMUCONbits.EDG1STAT = 0;       // Stop charging circuit
        IFS0bits.AD1IF = 0;             // make sure A/D Int not set
        AD1CON1bits.SAMP = 0;           // and begin A/D conv.
        
        while(!AD1CON1bits.DONE);       // Wait for A/D convert complete
        
        Vread = ADC1BUF0;               // Get the value from the A/D
        IFS0bits.AD1IF = 0;             // Clear A/D Interrupt Flag
        VTot += Vread;                  // Add the reading to the total

    }

    Vavg = (VTot/10);                   // Average of the 10 measurements
    c = (.550e-6 * 916 * (1/4e6))/(3.3*Vavg/1024);
    
    char str_capacitance[10];
    sprintf(str_capacitance, "%.9f", c);
    Disp2String(str_capacitance);
    Disp2String(" F");
    XmitUART2('\n', 1);
    XmitUART2('\r', 1);
    VTot = 0;
   
 }

unsigned int CTMUCalibrate(void) {
    
    CTMUCONbits.CTMUEN = 0;             // Disable the CTMU
    CTMUCONbits.TGEN = 0;               // Configured for current generation
    CTMUCONbits.EDGEN = 0;              // Edges are disabled
    CTMUCONbits.IDISSEN = 0;            // Current source is grounded (discharge enabled)
    CTMUICONbits.ITRIM = 0b000000;      // No trim
    CTMUCONbits.CTTRIG = 0;             // Trigger output disabled
    
    CTMUICONbits.IRNG = 0b11;             // 100 x base current
    
    TRISBbits.TRISB1 = 1;
    AD1PCFG = 0xFFF7;
    AD1CHSbits.CH0SA = 0b00011;         // Positive input is AN5/RA3/pin8 for MUXA
    
    AD1CON1 = 0x8000;
    AD1CON2 = 0x0000;
    AD1CON3 = 0x0000;
    
    CTMUCONbits.CTMUEN = 1;             // Enabled the CTMU
    CTMUCONbits.EDG1STAT = 1;           // Enable current source
    AD1CON1bits.SAMP = 1;               // Manual sampling start
    
    // Delay to discharge sample cap
    unsigned int x;
    for(x=0; x<2000; x++);
    
    IFS0bits.AD1IF = 0;                 // Clear ADC interrupt
    
    AD1CON1bits.SAMP = 0;               // Begin A/D conversion
    while(AD1CON1bits.DONE == 0);        // Wait for A/D conversion to complete
        
    CTMUCONbits.EDG1STAT = 0;          // Disbale current source
    IFS0bits.AD1IF = 0;                 // Clear ADC interrupt
            
    unsigned int result = ADC1BUF0;
    
    CTMUCONbits.CTMUEN = 0;                 //  Disable the CTMU
    
    return result;
           
}