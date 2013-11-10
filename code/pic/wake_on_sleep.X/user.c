/*
 ==============================================================================
 Name        : user.c
 Date        : Aug 6, 2013
 ==============================================================================

 BSD License
 -----------

 Copyright (c) 2013, and Kevin Fodor, All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 - Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

 - Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 - Neither the name of Kevin Fodor nor the names of
 its contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.

 NOTICE:
 SOME OF THIS CODE MAY HAVE ELEMENTS TAKEN FROM OTHER CODE WITHOUT ATTRIBUTION.
 IF THIS IS THE CASE IT WAS DUE TO OVERSIGHT WHILE DEBUGGING AND I APOLOGIZE.
 IF ANYONE HAS ANY REASON TO BELIEVE THAT ANY OF THIS CODE VIOLATES OTHER
 LICENSES PLEASE CONTACT ME WITH DETAILS SO THAT I MAY CORRECT THE SITUATION.

 ==============================================================================
 */

// Compiler specific includes
#if defined(__XC)
#include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
#include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C) || defined (__MINGW32__)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

// Module include
#include "user.h"

// Local declarations

// Implementation

/*! \brief init
 */
void init(void)
{
    /* Initialize User Ports/Peripherals/Project here */

#if (__18F45K20 == 1) || (_18F45K20 == 1)

    // Configure internal clock to 8MHz
    OSCCON = 0b01100000;// IRCFx = 110

    // Use HFINTOSC as clock source, 31.25kHz
    // 16MHz / 512 = 31.25kHz
    OSCTUNE = 0b10000000;// Minimum frequency

    // Wait for clock to become stable
    while (!OSCCONbits.IOFS);

    // INTCON: INTERRUPT CONTROL REGISTER
    INTCONbits.INT0IE = 1; //1 = Enables the INT0 interrupt
    INTCONbits.INT0IF = 0; //clear
    INTCONbits.TMR0IF = 0;// clear roll-over interrupt flag

    // INTCON2: INTERRUPT CONTROL 2 REGISTER
    INTCON2bits.RBPU = 0; // enable PORTB internal pullups

    // ANSEL: ANALOG SELECT REGISTER 1
    ANSEL  = 0x00;// AN0-7 are digital inputs
    // ANSELH: ANALOG SELECT REGISTER 2
    ANSELH = 0x00;// AN8-12 are digital inputs (AN12 on RB0)

    // Init Port-B (8-bits)
    WPUB = 0b00000001;// enable pull up on RB0
    TRISB = 0b00000001;// PORTB bit 7:1 are outputs, 0 is input
    LATB = 0b00000000;// All outputs [7:1] are low

    // Init Port-D (8-bits)
    TRISD = 0b00000000;// PORTD bits 7:0 are outputs
    LATD = 0b00000000;// All outputs [7:0] are low

    // Init Timer0 (Main-loop clock)

    TMR0H = 0;// clear timer - always write upper byte first
    TMR0L = 0;

    // T0CON: TIMER0 CONTROL REGISTER
    T0CON = 0b11000111;
    /*
    T0CONbits.TMR0ON = 1; //1 = Enables Timer0
    T0CONbits.T08BIT = 1; //1 = Timer0 is configured as an 8-bit timer/counter
    T0CONbits.T0CS = 0; //0 = Internal instruction cycle clock (CLKOUT)
    T0CONbits.T0SE = 0; //0 = Increment on low-to-high transition on T0CKI pin
    T0CONbits.PSA = 0; //Timer0 prescaler is assigned. Timer0 clock input comes from prescaler output.
    T0CONbits.T0PS = 0b111; //1:256 prescale value
    */

#elif (__16F1823 == 1) || (_16F1823 == 1)

    // OSCCON: OSCILLATOR CONTROL REGISTER
    OSCCON = 0b00111000;
    /*
     OSCCONbits.SPLLEN = 0;// 4x PLL is disabled
     OSCCONbits.IRCF = 0b0111;// 500 kHz MF (default upon Reset)
     OSCCONbits.SCS = 0b00;// Clock determined by FOSC<2:0> in Configuration Word 1.
     */

    // OSCTUNE: OSCILLATOR TUNING REGISTER
    OSCTUNE = 0b00000000;// Oscillator module is running at the
                         // factory-calibrated frequency.

    // TODO: Wait for clock to become stable
    //while (!OSCCONbits.IOFS);

    // OPTION_REG: OPTION REGISTER
    OPTION_REG = 0b00000011;
    /*
     OPTION_REGbits.nWPUEN = 0;// Enabled by WPUx latch values
     OPTION_REGbits.INTEDG = 0;// Falling edge of RB0/INT
     OPTION_REGbits.TMR0CS = 0;// Internal CLK (FOSC/4)
     OPTION_REGbits.TMR0SE = 0;// Increment on low-high (unused)
     OPTION_REGbits.PSA = 0;// Prescaler is assigned to the Timer0 module
     OPTION_REGbits.PS = 0b011;// 1 : 16
     */

    // CPSCON0: CAPACITIVE SENSING CONTROL REGISTER 0
    CPSCON0bits.T0XCS = 0;// Timer0 clock source is controlled by the
    // core/Timer0 module and is FOSC/4

    // INTCON: INTERRUPT CONTROL REGISTER
    INTCON = 0b00001000;
    /*
     INTCONbits.GIE = 0;// Disables all interrupts
     INTCONbits.PEIE = 0;// Disables all peripheral interrupts
     INTCONbits.TMR0IE = 0;// Disables the Timer0 interrupt
     INTCONbits.INTE = 0;// Disables the INT external interrupt
     INTCONbits.IOCIE = 1;// Enables the interrupt-on-change
     INTCONbits.TMR0IF = 0;// TMR0 register did not overflow(clear)
     INTCONbits.INTF = 0;// The INT external interrupt did not occur
     */

    // IOCAN: INTERRUPT-ON-CHANGE PORTA NEGATIVE EDGE REGISTER
    IOCAN = 0b00000000;// Interrupt-on-Change enabled on the pin for
    // a negative going edge. Associated Status bit and interrupt
    // flag will be set upon detecting an edge.

    // IOCAP: INTERRUPT-ON-CHANGE PORTA POSITIVE EDGE REGISTER
    IOCAP = 0b00010000;// Interrupt-on-Change enabled on the pin for
    // a positive going edge. Associated Status bit and interrupt
    // flag will be set upon detecting an edge.

    // Init Port-A (6-bits)
    WPUA = 0b00010000;// enable pull up on RA4
    ANSELA = 0b00000000;// ANSA0-4 are digital inputs
    TRISA = 0b00010000;// PORTA bit 3:0,5 are outputs, 4 is input
    LATA = 0b00000000;// All outputs [3:0,5] are low

    // Init Port-C (6-bits)
    WPUC = 0b00000000;// disable pull up on port-C [5:0]
    ANSELC = 0b00000000;// ANSC0-3 are digital inputs
    TRISC = 0b00000000;// PORTC bits 5:0 are outputs
    LATC = 0b00000000;// All outputs [5:0] are low

#elif defined __MINGW32__

    // Nothing to configure

#else

#error Error! You must create definitions for this processor.

#endif
    return;
}

