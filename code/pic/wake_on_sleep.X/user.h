/*
 ==============================================================================
 Name        : user.h
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

#ifndef USER_H_
#define USER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* ************************************************************************** */
/*!
 \defgroup user

 \brief These APIs and definitions are for the User module.
 */
/* ************************************************************************** */

#ifndef LOW_BYTE
#define LOW_BYTE(x)     ((unsigned char)((x)&0xFF))
#endif

#ifndef HIGH_BYTE
#define HIGH_BYTE(x)    ((unsigned char)(((x)>>8)&0xFF))
#endif

#if (__18F45K20 == 1) || (_18F45K20 == 1)

#define SYS_FREQ        (8000000L) // Hz
#define FOSC            SYS_FREQ
#define _XTAL_FREQ      SYS_FREQ
#define FCY             (SYS_FREQ/4)

#define TMR0_PRESCALE   (256) // 1:256 See T0CON: TIMER0 CONTROL REGISTER

// Definitions for clock timer and delay
#define TMR_CLOCK_USEC ((USEC_PER_SEC * TMR0_PRESCALE) / FCY) // 128 usec
#define TMR_COUNT (256 - (USEC_PER_TICK / TMR_CLOCK_USEC)) // 256 - 78.125
#define TIMER_EXPIRED (INTCONbits.TMR0IF)
#define TIMER_RESET (TMR0 = TMR_COUNT);INTCONbits.TMR0IF = 0

// Definitions for GPIO

#define HEARTBEAT   (0b00000001) // RD0
#define SB0         (0b00000010) // RD1
#define SB1         (0b00000100) // RD2

// GPIO-SPI
#define MCLK        (0b00001000) // RD3
#define MOSI        (0b00010000) // RD4
#define MISO        (0b00100000) // RD5
#define nCS         (0b01000000) // RD6

// GPIO Speaker
#define PWM_TRIS     (TRISDbits.TRISD7) // RD7

// GPIO Ports
#define HEARTBEAT_PORT (LATD)
#define STATE_PORT     (LATD)

#define SPI_MCLK_PORT  (LATD)
#define SPI_MOSI_PORT  (LATD)
#define SPI_MISO_PORT  (LATD)
#define SPI_nCS_PORT   (LATD)

// Input signals
#define nAWAKE (PORTBbits.INT0) // INT0
#define nAWAKE_CLEAR (INTCONbits.INT0IF = 0)

// Current State Mask
#define STATE_MASK (SB0 | SB1)
#define STATE_BITS_SHIFT (1)

#elif (__16F1823 == 1) || (_16F1823 == 1)

#define SYS_FREQ        (500000L) // Hz
#define FOSC            SYS_FREQ
#define _XTAL_FREQ      SYS_FREQ
#define FCY             (SYS_FREQ/4)

#define TMR0_PRESCALE   (16) // 1:16 See OPTION_REG (PS)

// Definitions for clock timer and delay
#define TMR_CLOCK_USEC ((USEC_PER_SEC * TMR0_PRESCALE) / FCY) // 128 usec
#define TMR_COUNT (256 - (USEC_PER_TICK / TMR_CLOCK_USEC)) // 256 - 78.125
#define TIMER_EXPIRED (INTCONbits.TMR0IF)
#define TIMER_RESET (TMR0 = TMR_COUNT);INTCONbits.TMR0IF = 0

// Definitions for GPIO

#define HEARTBEAT   (0b00100000) // RA5
#define SB0         (0b00001000) // RC3
#define SB1         (0b00010000) // RC4

// GPIO-SPI
#define MCLK        (0b00000100) // RC2
#define MOSI        (0b00000010) // RC1
#define MISO        (0b00000001) // RC0
#define nCS         (0b00000100) // RA2

// GPIO Speaker
#define PWM_TRIS     (TRISCbits.TRISC5) // RC5

// GPIO Ports
#define HEARTBEAT_PORT (LATA)
#define STATE_PORT     (LATC)

#define SPI_MCLK_PORT  (LATC)
#define SPI_MOSI_PORT  (LATC)
#define SPI_MISO_PORT  (LATC)
#define SPI_nCS_PORT   (LATA)

// Input signals
#define nAWAKE (PORTAbits.RA4) // RA4
#define nAWAKE_CLEAR (IOCAFbits.IOCAF4 = 0)

// Current State Mask
#define STATE_MASK (SB0 | SB1)
#define STATE_BITS_SHIFT (3)

#elif defined __MINGW32__

// Bring in some fake registers just so everything compiles
extern uint8_t dummy_port;

// Definitions for clock timer and delay
#define USEC_PER_TICK (10000)
#define TIMER_EXPIRED (dummy_port)
#define TIMER_RESET dummy_port = 0;

// Definitions for GPIO

#define HEARTBEAT   (0b00100000) // RA5
#define SB1         (0b00010000) // RC4
#define SB0         (0b00001000) // RC3

// GPIO-SPI
#define MCLK        (0b00000100) // RC2
#define MOSI        (0b00000010) // RC1
#define MISO        (0b00000001) // RC0
#define nCS         (0b00000100) // RA2

// GPIO Speaker
#define PWM_TRIS     (dummy_port) // RC5

// GPIO Ports
#define HEARTBEAT_PORT (dummy_port)
#define STATE_PORT     (dummy_port)

#define SPI_MCLK_PORT  (dummy_port)
#define SPI_MOSI_PORT  (dummy_port)
#define SPI_MISO_PORT  (dummy_port)
#define SPI_nCS_PORT   (dummy_port)

// Input signals
#define nAWAKE (dummy_port)
#define nAWAKE_CLEAR (dummy_port = 0)

// Current State Mask
#define STATE_MASK (SB0 | SB1)
#define STATE_BITS_SHIFT (3)

// CPU Sleep
#define SLEEP()

#else

#error Sorry! No defined configuration for this processor.

#endif

/* ************************************************************************** */
/*!
 \ingroup user

 \brief init

 I/O and Peripheral Initialization.

 \param[in] None.

 \return Nothing.

 */
/* ************************************************************************** */
void init(void);

#ifdef __cplusplus
}
#endif

#endif /* USER_H_ */

