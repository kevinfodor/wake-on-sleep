/*
 ==============================================================================
 Name        : registers.h
 Date        : Aug 12, 2013
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

#ifndef REGISTERS_H_
#define REGISTERS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#if defined (__MINGW32__)

extern uint8_t dummy_port;

#else

#error Do not use this file

#endif

#ifdef __cplusplus
}
#endif

#endif /* REGISTERS_H_ */
