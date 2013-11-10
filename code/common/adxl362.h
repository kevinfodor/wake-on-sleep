/*
 ==============================================================================
 Name        : adxl362.h
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

#ifndef ADXL362_H_
#define ADXL362_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* ************************************************************************** */
/*!
 \defgroup adxl362

 \brief These APIs and definitions are for the ADXL362 module.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/*!
 \ingroup adxl362

 \brief adxl362_init

 Initializes the accelerometer peripheral.

 \param[in] None.

 \return Nothing.

 */
/* ************************************************************************** */

void adxl362_init(void);

/* ************************************************************************** */
/*!
 \ingroup adxl362

 \brief adxl362_is_asleep

 Determines if the accelerometer is inactive (sleeping) or active (awake).

 \param[in] None.

 \return Nothing.

 */
/* ************************************************************************** */

bool adxl362_is_asleep(void);

/* ************************************************************************** */
/*!
 \ingroup adxl362

 \brief adxl362_autosleep

 Places the accelerometer into autosleep mode.

 \param[in] None.

 \return Nothing.

 */
/* ************************************************************************** */

void adxl362_autosleep(bool active);

#ifdef __cplusplus
}
#endif

#endif /* ADXL362_H_ */
