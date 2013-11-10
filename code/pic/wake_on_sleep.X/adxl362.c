/*
 ==============================================================================
 Name        : adxl362.c
 Date        : Aug 7, 2013
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
#elif defined(__MINGW32__)
#include <stdint.h>
#include <registers.h>   /* x86 Dummy Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C) || defined (__MINGW32__)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

// GPIO Includes
#include "user.h"

// Module include
#include "adxl362.h"

// Local declarations

#define SPI_NUM_BITS (8)    // Use 8-bit words
/* ADXL362 communication commands */
#define ADXL362_WRITE_REG           	0x0A

/* Registers */
#define ADXL362_REG_SOFT_RESET          0x1F
#define ADXL362_REG_THRESH_ACT_L        0x20
#define ADXL362_REG_POWER_CTL           0x2D

/* ADXL362 Reset settings */
#define ADXL362_RESET_KEY               0x52

/* ADXL362 Measureme Mode */
#define ADXL362_MEASURE                 (2 << 0)

/* ADXL362 Autosleep */
#define ADXL362_AUTOSLEEP               (1 << 2)

// NOTE: range +/-2g
#define ADXL362_THRESH_ACT             (125)    // 125mg
#define ADXL362_THRESH_INACT           (250)    // 250mg
// NOTE: sample rate 12.5Hz
#define ADXL363_TIME_ACT                (15)    // counts (~1.2 sec)
#define ADXL363_TIME_INACT             (125)    // counts (~10 sec)

static const uint8_t adxl362_reset_cmd[] =
{ ADXL362_WRITE_REG, ADXL362_REG_SOFT_RESET, ADXL362_RESET_KEY };

static const uint8_t adxl362_config_cmd[] =
{ ADXL362_WRITE_REG, ADXL362_REG_THRESH_ACT_L,

/*[20]*/LOW_BYTE(ADXL362_THRESH_ACT),
/*[21]*/HIGH_BYTE(ADXL362_THRESH_ACT),
/*[22]*/ADXL363_TIME_ACT,
/*[23]*/LOW_BYTE(ADXL362_THRESH_INACT),
/*[24]*/HIGH_BYTE(ADXL362_THRESH_INACT),
/*[25]*/LOW_BYTE(ADXL363_TIME_INACT),
/*[26]*/HIGH_BYTE(ADXL363_TIME_INACT),

/*[27]*/0x3f,
/*[28]*/0x00,
/*[29]*/0x80,
/*[2a]*/0x40,
/*[2b]*/0xC0,
/*[2c]*/0x10,
/*[2d]*/ADXL362_MEASURE

};

static const uint8_t adxl362_autosleep_cmd[2][3] =
{
        // Disable
        {
        ADXL362_WRITE_REG, ADXL362_REG_POWER_CTL,
        ADXL362_MEASURE },
        // Enable
        {
        ADXL362_WRITE_REG, ADXL362_REG_POWER_CTL,
        ADXL362_MEASURE | ADXL362_AUTOSLEEP } };

// Implementation

static void adxl362_xfer(uint8_t const * data, uint8_t num_bytes);

/*! \brief adxl362_init
 */
static void adxl362_xfer(uint8_t const * data, uint8_t num_bytes)
{
    uint8_t bits, shiftOut = 0;

    SPI_nCS_PORT &= ~nCS; // Active-low

    // For each byte to xfer...
    while (num_bytes--)
    {
        // Initialize number of bits to xfer
        bits = SPI_NUM_BITS;

        // Initialize shift out register and consume
        shiftOut = *data++;

        // For each bit per byte, MSB first
        while (bits--)
        {
            // Determine bit state
            if (shiftOut & 0x80)
            {
                SPI_MOSI_PORT |= MOSI; // Active
            }
            else
            {
                SPI_MOSI_PORT &= ~MOSI; // Inactive
            }

            // Clock in data
            SPI_MCLK_PORT |= MCLK;
            SPI_MCLK_PORT &= ~MCLK;

            // Next bit (MSB first)
            shiftOut <<= 1;
        }
    }

    SPI_nCS_PORT |= nCS; // Inactive, active-low

    return;
}

/*! \brief adxl362_init
 */
void adxl362_init(void)
{
    // Configure GPIO for SPI

    // Initialize SPI signal conditions
    SPI_nCS_PORT |= nCS; // Inactive
    SPI_MCLK_PORT &= ~MCLK; // Idle
    SPI_MOSI_PORT &= ~MOSI; // Inactive

    // Reset ADXL362
    adxl362_xfer(adxl362_reset_cmd, sizeof(adxl362_reset_cmd));

    // Program ADXL362 (Wake-on-Sleep)
    adxl362_xfer(adxl362_config_cmd, sizeof(adxl362_config_cmd));

    return;
}

/*! \brief adxl362_is_asleep
 */
bool adxl362_is_asleep(void)
{
    bool is_asleep;

    is_asleep = nAWAKE;

    return is_asleep;
}

/*! \brief adxl362_autosleep
 */
void adxl362_autosleep(bool active)
{
    uint8_t index = ((active == false) ? 0 : 1);

    // Program ADXL362 (Autosleep)
    adxl362_xfer(adxl362_autosleep_cmd[index],
            sizeof(adxl362_autosleep_cmd[index]));

    return;
}
