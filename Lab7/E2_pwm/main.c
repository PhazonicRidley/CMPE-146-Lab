/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/
#define RGBLED_PORT  GPIO_PORT_P2
#define RLED_PIN  GPIO_PIN0

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#define RAMP_TIME 2500U
#define PWM_FREQ 500U

#define AMOUNT_LEVELS ((uint32_t) ((RAMP_TIME * PWM_FREQ) / 1000U) )

static void delay_cycles(uint32_t cycles)
{
    // Burn cycles by injecting nops
    while (cycles--) {
        __asm(" nop");
    }
}


void software_pwm(uint32_t period_cycles, uint32_t duty_cycle) {
    uint32_t amt_on = (period_cycles * duty_cycle) / AMOUNT_LEVELS;
    uint32_t amt_off = period_cycles - amt_on;

    if (amt_on) {
        MAP_GPIO_setOutputHighOnPin(RGBLED_PORT, RLED_PIN);
        delay_cycles(amt_on);
    }

    if (amt_off) {
        MAP_GPIO_setOutputLowOnPin(RGBLED_PORT, RLED_PIN);
        delay_cycles(amt_off);
    }
}


int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    uint32_t cycles_per_us = MAP_CS_getMCLK() / 1000000UL;
    uint32_t pwm_period_us = 1000000UL / PWM_FREQ;
    uint32_t pwm_period_cycles = pwm_period_us * cycles_per_us;

    printf("Ramp‑time (one‑way): %u ms\n", RAMP_TIME);
    printf("PWM frequency: %u Hz\n", PWM_FREQ);
    printf("PWM period: %lu µs\n", (uint64_t)pwm_period_cycles);
    printf("PWM resolution: %u levels\n", AMOUNT_LEVELS + 1);
    printf("Control cycle: %u ms\n\n", 2 * RAMP_TIME);

    MAP_GPIO_setAsOutputPin(RGBLED_PORT, RLED_PIN);
    MAP_GPIO_setOutputLowOnPin(RGBLED_PORT, RLED_PIN);


    while(1)
    {
        uint32_t duty;                       /* 1. declare first */
        for (duty = 0; duty <= AMOUNT_LEVELS; ++duty)   /* 2. plain assignment */
        {
            software_pwm(pwm_period_cycles, duty);
        }

        for (duty = AMOUNT_LEVELS - 1; duty > 0; --duty)   /* 2. plain assignment */
        {
            software_pwm(pwm_period_cycles, duty);
        }
    }

}

