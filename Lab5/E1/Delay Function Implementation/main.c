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
#include <math.h>

#define CURRENT_TIME_0 ((uint32_t)MAP_Timer32_getValue(TIMER32_0_BASE))
#define CURRENT_TIME_1 ((uint32_t)MAP_Timer32_getValue(TIMER32_1_BASE))


inline void delay_ms(uint32_t count)
{
    uint32_t prev = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint64_t ticks_to_wait = ((uint64_t)MAP_CS_getMCLK() * count) / 1000;
    uint64_t total_elapsed = 0;
    uint32_t current;

    while(total_elapsed < ticks_to_wait)
    {
        current = MAP_Timer32_getValue(TIMER32_0_BASE);

        if(prev >= current) {
            total_elapsed += (uint64_t)(prev - current);
        } else {
            total_elapsed += (uint64_t)(prev + ((uint32_t)0xFFFFFFFF - current)); // Handle underflow
        }

        prev = current;
    }
}


static inline uint64_t delta_time_us(uint64_t start, uint64_t end) {

    uint32_t tick_delta = start - end;
//    if (end <= start) {
//
//    } else {
//        tick_delta = start + ((uint32_t)0xFFFFFFFF - end);
//    }

    uint64_t us = ((uint64_t)tick_delta * 1000000) / MAP_CS_getMCLK();
    return us;
}


#define SIZE 10

int main(void)
{
    uint32_t t0, t1;
    uint32_t delay_v[SIZE] = {5000, 2000, 1000, 50, 20, 10, 5, 2, 1, 0};
    uint64_t delay_data[SIZE];

    int i;
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    MAP_Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_1_BASE, 0);


    for (i = 0; i < SIZE; i++) {
        t0 = (uint64_t)CURRENT_TIME_1;
        delay_ms(delay_v[i]);
        t1 = (uint64_t)CURRENT_TIME_1;
        delay_data[i] = delta_time_us(t0, t1);
    }

    for (i = 0; i < SIZE; i++) {
        uint64_t expected_us = (uint64_t)delay_v[i] * 1000;
        uint64_t measured_us = delay_data[i];
        uint64_t error_us = abs(expected_us - measured_us);
        printf("Delay expected value: %lums, Measured value: %lluus, Error delta: %lluus\n", delay_v[i], delay_data[i], error_us);
    }

    while(1);

}
