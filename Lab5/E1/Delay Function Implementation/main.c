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
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define CURRENT_TIME ((uint32_t)MAP_Timer32_getValue(TIMER32_0_BASE))

void delay_ms(uint32_t count)
{
    uint64_t ticks_to_wait = ((uint64_t)MAP_CS_getMCLK() * count) / 1000;
    uint64_t total_elapsed = 0;
    uint64_t prev = (uint64_t)CURRENT_TIME;
    uint64_t current;

    while(total_elapsed < ticks_to_wait)
    {
        uint64_t elapsed;
        current = (uint64_t)CURRENT_TIME;

        if(prev >= current) {
            elapsed = prev - current;
        } else {
            elapsed = prev + ((uint32_t)0xFFFFFFFF - current); // Handle underflow
        }

        total_elapsed += elapsed;
        prev = current;
    }
}


int main(void)
{
    const uint32_t size = 10;
    uint32_t t0, t1, delay;
    uint64_t freq = 3000;
    uint32_t delay_v[size] = {5000, 2000, 1000, 50, 20, 10, 5, 2, 1, 0};
    int i;
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    MAP_Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_1_BASE, 0);

    for(i = 0; i < size; i++ )
    {
        t0 = MAP_Timer32_getValue(TIMER32_1_BASE);
        delay_ms(delay_v[i]);
        t1 = MAP_Timer32_getValue(TIMER32_1_BASE);
        delay = (t0-t1)/freq;
        printf("Delay time at %d: %d\n", delay_v[i], delay);
        printf("Delay time difference at %d: %d\n", delay_v[i], delay - delay_v[i]);
    }
}
