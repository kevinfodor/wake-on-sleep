/*
 ==============================================================================
 Name        : wake_on_sleep.c
 Date        : Aug 06, 2013
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

#include <stddef.h>

// CPU specific include
#include "../pic/wake_on_sleep.X/user.h"

// Project includes
#include "pwm.h"
#include "adxl362.h"
#include "wake_on_sleep.h"

// Time base defines
#define USEC_PER_TICK (10000) // 10.000 msec

// Common time definitions
#define USEC_PER_SEC    (1000000)
#define MSEC_PER_USEC   (1000)
#define SEC_PER_MSEC    (1000)

// Timeout definitions
#define EXPIRED (0)
#define ALERT_TIMEOUT_USEC                      (10000000)  // 10 sec
#define HEARTBEAT_TIMEOUT_USEC                  (200000)    // 200 msec
#define SOUND_ON_USEC 				(250000)    // 250 msec
#define SOUND_OFF_USEC 				(750000)    // 750 msec
#define INIT_BEEP_USEC                          (125000)    // 125 msec
#define SLEEP_WAIT_USEC                         (500000)    // 500 msec

// Power-on "announcement" beeps
#define NUM_ANNOUNCE_BEEPS (3)

// Timeout counter definitions
#define ONE_SECOND_TIMEOUT_COUNT ((SEC_PER_MSEC * MSEC_PER_USEC) / USEC_PER_TICK)
#define ALERT_TIMEOUT_COUNT (ALERT_TIMEOUT_USEC / USEC_PER_TICK)
#define HEARTBEAT_TIMEOUT_COUNT (HEARTBEAT_TIMEOUT_USEC / USEC_PER_TICK)
#define SOUND_ON_TIMEOUT_COUNT (SOUND_ON_USEC / USEC_PER_TICK)
#define SOUND_OFF_TIMEOUT_COUNT (SOUND_OFF_USEC / USEC_PER_TICK)
#define ANNOUNCE_BEEP_TIMEOUT_COUNT (INIT_BEEP_USEC / USEC_PER_TICK)
#define SLEEP_WAIT_COUNT (SLEEP_WAIT_USEC / USEC_PER_TICK)

/*
 * Controller States.
 */
typedef enum _controller_state_t
{
    // Note: sleep state is defined first
    // since we want the LEDs to be off in this mode.
    controller_sleep = 0,
    controller_init,
    controller_alert,
    controller_unknown

} controller_state_t, *controller_state_ptr_t;

/*
 * Controller state variables.
 */
typedef struct _controller_state_var_t
{
    controller_state_t previous;
    controller_state_t current;
} controller_fsm_state_vars_t, *controller_state_var_ptr_t;

/*
 * Controller state-transition table.
 */
typedef struct _controller_fsm_state_t
{
    // State
    controller_state_t state;

    // Functions
    void (*enter)(void);
    controller_state_t (*run)(void);
    void (*exit)(void);

} controller_fsm_tabel_t, *controller_fsm_state_ptr_t;

/*
 * Controller init state-data.
 */
typedef struct _controller_init_state_data_t
{
    uint8_t beep_count;
    uint8_t sound_count;
} controller_init_state_data_t, *controller_init_state_data_ptr_t;

/*
 * Controller sleep state-data.
 */
typedef struct _controller_sleep_state_data_t
{
    uint8_t sleep_wait_count;
} controller_sleep_state_data_t, *controller_sleep_state_data_ptr_t;

/*
 * Controller alert state-data.
 */
typedef struct _controller_alert_state_data_t
{
    uint16_t alert_count;
    uint8_t sound_count;
    uint8_t alert_profile_index;
} controller_alert_state_data_t, *controller_alert_state_data_ptr_t;

/*
 * Controller state-data union.
 * All structures here are shared state variables
 * which must be initialized upon state entry.
 */
typedef union _controller_fsm_data_t
{
    controller_init_state_data_t init;
    controller_sleep_state_data_t sleep;
    controller_alert_state_data_t alert;

} controller_fsm_data_t, *controller_fsm_data_ptr_t;

/*
 * Controller finite-state machine definition.
 */
typedef struct _controller_fsm_t
{
    controller_fsm_state_vars_t state;
    controller_fsm_data_t data;
    controller_fsm_tabel_t const * table;
} controller_fsm_t, *controller_fsm_ptr_t;

/*
 * Alert profile structure definition.
 */
typedef struct _alert_profile_t
{
    uint8_t range;
    uint8_t count[2]; // on/off

} alert_profile_t, *alert_profile_ptr_t;

/*
 * Local Function Declarations.
 */

static void update_heartbeat(void);

// init-state prototypes
static void fsm_init_enter(void);
static controller_state_t fsm_init_run(void);
static void fsm_init_exit(void);

// sleep-state prototypes
static void fsm_sleep_enter(void);
static controller_state_t fsm_sleep_run(void);
static void fsm_sleep_exit(void);

// alert-state prototypes
static void fsm_alert_enter(void);
static controller_state_t fsm_alert_run(void);
static void fsm_alert_exit(void);

/*
 * Local static variable declarations.
 */

static const alert_profile_t alert_profile[] =
{
{ 3,
{ SOUND_ON_TIMEOUT_COUNT, SOUND_OFF_TIMEOUT_COUNT } },
{ 1,
{ SOUND_ON_TIMEOUT_COUNT, SOUND_OFF_TIMEOUT_COUNT / 3 } },
{ 0,
{ SOUND_ON_TIMEOUT_COUNT * 4, 0 } } };

static controller_fsm_tabel_t const controller_fsm_table[] =
{
// Note: These must be listed in numerical order by state value.
        { controller_sleep, fsm_sleep_enter, fsm_sleep_run, fsm_sleep_exit },
        { controller_init, fsm_init_enter, fsm_init_run, fsm_init_exit },
        { controller_alert, fsm_alert_enter, fsm_alert_run, fsm_alert_exit } };

static controller_fsm_t controller_fsm;

/*
 * Implementation
 */

/*! \brief update_heartbeat
 */
static void update_heartbeat(void)
{
    static uint16_t heartbeat_count = HEARTBEAT_TIMEOUT_COUNT;

    if (heartbeat_count-- == EXPIRED)
    {
        // Reset heart beat timeout
        heartbeat_count = HEARTBEAT_TIMEOUT_COUNT;

        // Toggle heart beat
        HEARTBEAT_PORT ^= HEARTBEAT;
    }

    return;
}

/*! \brief fsm_init_enter
 */
static void fsm_init_enter(void)
{
    controller_init_state_data_ptr_t init_data = &controller_fsm.data.init;

    // Initialize the PWM module. The PWM module is used
    // to drive the speaker(buzzer)
    pwm_init();

    // Initialize the ADXL362 for autonomous operation
    adxl362_init();

    // Initialize the "Ready Announcement"
    init_data->sound_count = ANNOUNCE_BEEP_TIMEOUT_COUNT;
    init_data->beep_count = NUM_ANNOUNCE_BEEPS;
    pwm_start(); // Start the PWM module.

    return;
}

/*! \brief fsm_init_run
 */
static controller_state_t fsm_init_run(void)
{
    controller_init_state_data_ptr_t init_data = &controller_fsm.data.init;
    controller_state_t state = controller_init;

    // Announce "ready"
    if (init_data->sound_count-- == EXPIRED)
    {
        // Sound on?
        if (pwm_is_on() == true)
        {
            // Sound off
            pwm_stop();

            // One beep generated.
            if (--init_data->beep_count == 0)
            {
                // Transition to the sleep state and wait
                state = controller_sleep;
            }
        }
        else
        {
            // Sound on
            pwm_start();
        }

        // Restart beep-on timer
        init_data->sound_count = ANNOUNCE_BEEP_TIMEOUT_COUNT;
    }

    return state;
}

/*! \brief fsm_init_exit
 */
static void fsm_init_exit(void)
{
    return;
}

/*! \brief fsm_sleep_enter
 */
static void fsm_sleep_enter(void)
{
    controller_sleep_state_data_ptr_t sleep_data = &controller_fsm.data.sleep;

    // Initialize sleep wait timeout
    sleep_data->sleep_wait_count = SLEEP_WAIT_COUNT;

    return;
}

/*! \brief fsm_sleep_run
 */
static controller_state_t fsm_sleep_run(void)
{
    controller_state_t state = controller_sleep;
    controller_sleep_state_data_ptr_t sleep_data = &controller_fsm.data.sleep;

    // Wait for controller to settle
    if (sleep_data->sleep_wait_count-- == EXPIRED)
    {
        // Put accelerometer into auto-sleep mode
        adxl362_autosleep(true);
        
        // Turn off heart beat
        HEARTBEAT_PORT &= ~HEARTBEAT;
        
        // Put controller into sleep mode
        // *** SLEEP until nAWAKE goes high ***
        // *** ZZZzzz...
        nAWAKE_CLEAR; // clear interrupt
        SLEEP();

        // ...huh? I'm awake!
        // We are not active, sound the alert!
        state = controller_alert;
    }

    return state;
}

/*! \brief fsm_sleep_exit
 */
static void fsm_sleep_exit(void)
{
    // Take accelerometer out of auto-sleep mode
    adxl362_autosleep(false);

    // Exit sleep mode
    return;
}

/*! \brief fsm_inactive_enter
 */
static void fsm_alert_enter(void)
{
    controller_alert_state_data_ptr_t alert_data = &controller_fsm.data.alert;

    // Initialize state variables
    alert_data->alert_count = ALERT_TIMEOUT_COUNT;
    alert_data->sound_count = SOUND_ON_TIMEOUT_COUNT;
    alert_data->alert_profile_index = 0;

    // Start the PWM module.
    pwm_start();

    return;
}

/*! \brief fsm_inactive_run
 */
static controller_state_t fsm_alert_run(void)
{
    bool awake;
    controller_state_t state = controller_alert;
    controller_alert_state_data_ptr_t alert_data = &controller_fsm.data.alert;

    // Get current accelerometer state
    awake = !adxl362_is_asleep();

    // Any activity or timeout, go back to sleep
    if ((awake) || (alert_data->alert_count == EXPIRED))
    {
        // Go back to sleep
        state = controller_sleep;
    }
    else
    {
        // Adjust alert timeout counter
        --alert_data->alert_count;

        // Sound timeout?
        if (alert_data->sound_count-- == EXPIRED)
        {
            uint8_t seconds_remaining = (alert_data->alert_count / 100) + 1;

            // Update alert profile
            if ((seconds_remaining
                    <= alert_profile[alert_data->alert_profile_index].range)
                    && (alert_data->alert_profile_index
                            < sizeof(alert_profile) / sizeof(alert_profile_t)))
            {
                // Choose next profile
                alert_data->alert_profile_index++;
            }

            // Sound on?
            if (pwm_is_on() == false)
            {
                // Configure counter and turn on sound.
                alert_data->sound_count =
                        alert_profile[alert_data->alert_profile_index].count[0];
                if (alert_data->sound_count > 0)
                {
                    pwm_start();
                }
            }
            // Sound off
            else
            {
                // Configure counter and turn off sound.
                alert_data->sound_count =
                        alert_profile[alert_data->alert_profile_index].count[1];
                if (alert_data->sound_count > 0)
                {
                    pwm_stop();
                }
            }
        }
    }

    return state;
}

/*! \brief fsm_inactive_exit
 */
static void fsm_alert_exit(void)
{
    // Stop the PWM module.
    pwm_stop();

    return;
}

/*! \brief main
 */
int main(void)
{

    init();

    // Initialize state variables.
    controller_fsm.state.previous = controller_unknown;
    controller_fsm.state.current = controller_init;

    // Link in state transition table.
    controller_fsm.table = controller_fsm_table;

    do
    {
        // Wait for periodic timeout
        while (!TIMER_EXPIRED);
        TIMER_RESET;

        update_heartbeat();

        /*
         * Controller Finite State Machine
         */

        // Entry
        if (controller_fsm.state.previous != controller_fsm.state.current)
        {
            controller_fsm_table[controller_fsm.state.current].enter();
            controller_fsm.state.previous = controller_fsm.state.current;
        }

        // Run
        controller_fsm.state.current =
                controller_fsm_table[controller_fsm.state.current].run();

        // Exit
        if (controller_fsm.state.previous != controller_fsm.state.current)
        {
            controller_fsm_table[controller_fsm.state.previous].exit();
        }

        // Clear and set state bits
        STATE_PORT &= ~STATE_MASK;
        STATE_PORT |=
                ((controller_fsm.state.current & 0x03) << STATE_BITS_SHIFT);

    } while (true);

    return 0;
}
