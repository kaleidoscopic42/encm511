/* 
 * File:   ADCTest.c
 * Author: prabh
 *
 * Created on 17 de noviembre de 2021, 13:48
 */

#include "xc.h"
#include "UART2.h"
#include <stdio.h>

void Delay_ms();

int active_flag = 0;

void ADCinit(void) {
    
    // AD1CON1 Settings
    AD1CON1bits.ADON = 0;           // Turn off ADC
    AD1CON1bits.ADSIDL = 0;         // Continue operation in idle mode
    AD1CON1bits.FORM = 0b00;        // Data output form set to "Integer"
    AD1CON1bits.SSRC = 0b111;       // Conversion is started automatically
    AD1CON1bits.ASAM = 1;           // Sampling begins automatically when last conversion ends
    AD1CON1bits.SAMP = 0;           // A/D cycle is holding
    AD1CON1bits.DONE = 0;           // A/D conversion is not done
    
    // AD2CON2 Settings 
    AD1CON2bits.VCFG = 0b000;       // Vref+ = AVDD, Vref- = AVSS
    AD1CON2bits.OFFCAL = 0;         // No calibration is done
    AD1CON2bits.CSCNA = 0;          // Do not scan input
    AD1CON2bits.SMPI = 0b0001;      // ADC interrupts occur at the completion of every second sample/convert sequence
    AD1CON2bits.BUFM = 0;           // Buffer configured as 1:16 word buffer
    AD1CON2bits.ALTS = 0;           // Always use MUXA input MUX settings
    
    // AD2CON3 Settings 
    AD1CON3bits.ADCS = 0b101101;    // ADCS conversion clock selects bits 46 Tcy. Where Tcy = 2 * Tosc
    AD1CON3bits.SAMC = 0b11111;     // Sample time = 31 * TAD = 31 * (1/) = 0.992 ms
 
    AD1CHS  = 0x0007;       // Configure input channels,
                            // S/H+ input is AN5,
                            // S/H- input is Vr- (AVss).
    
    IFS0bits.AD1IF = 0;     // Clear A/D conversion interrupt.
    IPC3bits.AD1IP = 7;     // Configure A/D interrupt priority bits
    
    // MUX settings
    AD1CHSbits.CH0NB = 0;       // CH0 MUXB -ve Input is VR-
    AD1CHSbits.CH0SB = 0b00101;  // Positive input is AN5/RA3/pin8 for MUXB
    AD1CHSbits.CH0NA = 0;       // CH0 -ve Input is VR-
    AD1CHSbits.CH0SA = 0b00101;  // Positive input is AN5/RA3/pin8 for MUXA
    
    // IO Port selection
    TRISAbits.TRISA3 = 1;       // Set pin8/AN5/RA3 as input
    TRISBbits.TRISB13 = 1;       // Set pin16/AN11/RB13 as input
    AD1PCFG = 0xF7DF;           // Set pin 8 (AN5) and pin 16 (AN11) as analog
    AD1CSSL = 0;                // Input scan disabled, 0x0000 is default state
    
    // Turn on ADC module
    AD1CON1bits.ADON = 1;

    return;
    
}

// Prints a bar with length that is proportional to analog voltage as well as hex value of analog voltage
//void print_hex(uint16_t value) {
//    
//    // Minimum one box is printed (for V = 0) and maximum 64 boxes (for V = VDD);
//    uint16_t num_of_boxes = value/16+1;
//    int i;
//    for(i = 0; i < num_of_boxes; ++i) {
//        Disp2String("#");
//    }
//    
//    // Display the hex value
//    Disp2String(" ");
//    Disp2Hex(value);
//    
//    // Go to next line
//    XmitUART2('\n', 1);
//    XmitUART2('\r', 1);
//    
//}

uint16_t get_ADC_measurement(int port) {
    
    AD1CHSbits.CH0SA = port;
    
    // Clear interrupt flag
    IFS0bits.AD1IF = 0;
    
    // Wait until sampling is done
    while(!IFS0bits.AD1IF) {};
    
    return ADC1BUF0;
 
}

// Returns an average of 500 samples taken 1ms apart
float get_average(int port) {
    
    active_flag = 1;
    
    double average = 0;
    int counter;
    for(counter = 0; counter < 500; ++counter) {
        average += get_ADC_measurement(port) * 0.002;
    }
    
    active_flag = 0;
    
    return average;
}

// Performs a single ADC measurement on AN5 (for voltage))
uint16_t get_AN5_measurement(void) {
    
    float avg_measurement = get_average(0b00101);
    
    Disp2String("Voltage Measurement: ");
    float voltage_conversion = avg_measurement*(3.3)/1024;
    char str_voltage[10];
    sprintf(str_voltage, "%.2f", voltage_conversion);
    Disp2String(str_voltage);
    Disp2String(" V");
    
    XmitUART2('\n', 1);
    XmitUART2('\r', 1);
   
    return avg_measurement;
    
}

// Performs a single ADC measurement on AN11 (for resistance))
uint16_t get_AN11_measurement(void) {
    
    float avg_measurement = get_average(0b01011);
    
    // The maximum resistance we can measure with 16 bits is 65535 ohms
    // To account for noise or other error measurements, we'll set our own maximum to 41667 ohms
    if(avg_measurement > 1000) {
        Disp2String("Maximum allowed resistance exceeded");
        XmitUART2('\n', 1);
        XmitUART2('\r', 1);
    } else {
        // Can only hold 16 bits, so needs to be divided by 16, then multiplied at the end
        int divided_resistance = avg_measurement * (975 / 16) / (1024 - avg_measurement);

        int actual_resistance = divided_resistance * 16;

        Disp2String("Resistance Measurement: ");
        Disp2Dec(actual_resistance);
        Disp2String(" Ohm");

        XmitUART2('\n', 1);
        XmitUART2('\r', 1);
    }

    return avg_measurement;
    
}

// ---[ This part of the code is not being used for this project ]---

//// Prints 10 averages taken over 1 second intervals
//uint16_t do_ADC(void) {
//    
//    // Line break before printing to help organize things on terminal
//    XmitUART2('\n', 1);
//    XmitUART2('\r', 1);
//    
//    // Active flag is used so that button presses are ignored during operation
//    active_flag = 1;
//    
//    // Print 10 averages 1 second apart
//    int second_counter;
//    for(second_counter = 0; second_counter < 10; ++second_counter) {
//        print_hex(get_average());
//    }
//    
//    // Allow button presses to be used again and go into idle mode
//    active_flag = 0;
//    Idle();
//}
//

// Function to check if do_ADC is currently operating
int check_active() {
    return active_flag;
}

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt (void) {
    IFS0bits.AD1IF = 0; //  Clear the ADC1 interrupt flag
}