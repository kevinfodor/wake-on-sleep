/*
 ==============================================================================
 Name        : pwm.c
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

// Target includes
#include "user.h"

// Module include
#include "pwm.h"

#define PWM_FREQ (2048) // 2.048 kHz

// Local declarations

// Implementation

/*! \brief pwm_init
 */
void pwm_init(void)
{
#if (__18F45K20 == 1) || (_18F45K20 == 1)

    // Set as output so PWM output drives it
    PWM_TRIS = 0;

    // Set up 8-bit Timer2 to generate the PWM period (frequency)
    // Prescale = 4, timer off, postscale not used with CCP module
    T2CONbits.T2OUTPS = 0b0000;
    T2CONbits.TMR2ON = 0b0;
    T2CONbits.T2CKPS = 0b01;

    // Timer 2 Period Register (PR2) = (<clk> / 4 / <prescale> / <freq>) - 1
    // Thus, the PWM frequency is:
    // 8MHz clock / 4 = 2MHz instruction rate.
    // (2MHz / <prescale>) / <PR2+1>)
    PR2 = (FCY / 4 / 2048) - 1;

    // The Duty Cycle is controlled by the ten-bit CCPR1L<7,0>:DC1B1:DC1B0
    // 50% Duty Cycle = 0.5 * (<PR2+1> * 4)
    // The 8 most significant bits of the value are needed here
    CCPR1L = (((((PR2+1) * 2) >> 2)) & 0xFF);

    // P1Mx = 01 Full-Bridge output forward, so we get the PWM
    // signal on P1D to LED7.  Only Single Output (00) is needed,
    // but the P1A pin does not connect to a demo board LED
    // The 2 least significant bits of the value are written
    // to the DC1B1 and DC1B0 bits in CCP1CON
    // CCP1Mx = 1100, PWM mode with P1D active-high.
    CCP1CONbits.P1M = 0b01;
    CCP1CONbits.DC1B = (((PR2+1) * 2) & 0x03);
    CCP1CONbits.CCP1M = 0b1100;

#elif (__16F1823 == 1) || (_16F1823 == 1)

#define TMR2_PRESCALE (1)

    // PWM Period = [(PR2)+ 1] * 4 * Tosc * TMR2PS
    // 1 / PWM_Freq = [(PR2)+ 1] * 4 * (1 / Fosc) * TMR2PS
    // PR2 = (Fosc / (4 * TMR2PS * PWM_Freq)) - 1
#define PWM_PERIOD ((FOSC / (4 * TMR2_PRESCALE * PWM_FREQ)) - 1)

    // Disable the CCP1 pin output driver by setting
    // the associated TRIS bit.
    PWM_TRIS = 1;

    // Load the PR2 register with the PWM period value.
    PR2 = PWM_PERIOD;

    // CCP1CON: CCP1 CONTROL REGISTER
    // Configure the CCP1 module for the PWM mode
    // by loading the CCP1CON register with the
    // appropriate values.

    // The Duty Cycle is controlled by the ten-bit CCPR1L<7,0>:DC1B1:DC1B0
    // 50% Duty Cycle = 1/2 * [(PWM_PERIOD)+ 1] * 4
    // Pulse Width = ([(PWM_PERIOD)+ 1] * 4) / 2
#define PULSE_WIDTH ((((PWM_PERIOD)+ 1) * 4) / 2)

    // Single output; P1A modulated; P1B, P1C, P1D assigned as port pins
    CCP1CONbits.P1M = 0b00;
    // These bits are the two LSbs of the PWM duty cycle. The eight MSbs
    // are found in CCPR1L.
    CCP1CONbits.DC1B = (PULSE_WIDTH & 0x03);
    // PWM mode: P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;

    // Load the CCPR1L register and the DC1B1 bits
    // of the CCP1CON register, with the PWM duty
    // cycle value.

    // The 8 most significant bits of the value are needed here
    CCPR1L = ((PULSE_WIDTH >> 2) & 0xFF);

    // Configure and start Timer2:
    // -Clear the TMR2IF interrupt flag bit of the
    //  PIR1 register.
    PIR1bits.TMR2IF = 0;

    // T2CON: TIMER2 CONTROL REGISTER

    // -Configure the T2CKPS bits of the T2CON
    // register with the Timer prescale value.
    T2CONbits.T2CKPS = 0b00;// Prescaler is 1
    // - Enable the Timer by setting the TMR2ON
    // bit of the T2CON register.
    T2CONbits.TMR2ON = 1;// Timer2 is on

    // Set up 8-bit Timer2 to generate the PWM period (frequency)
    // Prescale = 4, timer off, postscale not used with CCP module
    T2CONbits.T2OUTPS = 0b0000;// 1:1 Postscaler

#elif defined __MINGW32__

    // Nothing to configure

#else

#error Error! You must create definitions for this processor.

#endif
    return;
}

/*! \brief pwm_start
 */
void pwm_start(void)
{
#if (__18F45K20 == 1) || (_18F45K20 == 1) || \
    (__16F1823 == 1) || (_16F1823 == 1)

    // Enable the CCP1 pin output driver by clearing
    // the associated TRIS bit.
    PWM_TRIS = 0;

    // Timer 2 on
    T2CONbits.TMR2ON = 1;

#elif defined __MINGW32__

#else

#error Error! You must create definitions for this processor.

#endif
    return;
}

/*! \brief pwm_stop
 */
void pwm_stop(void)
{
#if (__18F45K20 == 1) || (_18F45K20 == 1) || \
    (__16F1823 == 1) || (_16F1823 == 1)

    // Check if the timer is on
    if(T2CONbits.TMR2ON == 1)
    {
        // Wait until the Timer overflows and the TMR2IF bit of the PIR1
        // register is set.
        while (PIR1bits.TMR2IF == 0);

        // Disable the CCP1 pin output driver by setting
        // the associated TRIS bit.
        PWM_TRIS = 1;

        // Timer 2 off
        T2CONbits.TMR2ON = 0;
    }

#elif defined __MINGW32__

#else

#error Error! You must create definitions for this processor.

#endif
    return;
}

/*! \brief pwm_is_on
 */
int pwm_is_on(void)
{
#if (__18F45K20 == 1) || (_18F45K20 == 1) || \
    (__16F1823 == 1) || (_16F1823 == 1)

    // Return timer state
    return (T2CONbits.TMR2ON == 1);

#elif defined __MINGW32__

    return false;
#else

#error Error! You must create definitions for this processor.

#endif
}
