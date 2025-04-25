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

#define CAP_STATE ((bool)(CAPTIO0CTL & 1 << 9))

typedef struct TimeData_s {
    double time_ms;
    double freq_khz;
} TimeData_t;

TimeData_t delta_time(uint32_t start, uint32_t end) {

    uint32_t tick_delta = abs(start - end); // Countdown timer
    double time_sec = (double)tick_delta / MAP_CS_getMCLK();
    TimeData_t res;
    res.time_ms = time_sec * pow(10, 3);
    res.freq_khz = 1 / res.time_ms;

    return res;
}

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

uint32_t measure_frequency() {
   MAP_Timer_A_clearTimer(TIMER_A2_BASE);
   delay_ms(3);
   uint16_t timerAvalue = MAP_Timer_A_getCounterValue(TIMER_A2_BASE);
   uint32_t freq = (timerAvalue * 1000) / 3;

   return freq;
}

int main(void)
{
//    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
//    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
//    uint32_t start, end;
//    start = CURRENT_TIME;
//    delay_ms(5000);
//    end = CURRENT_TIME;
//    TimeData_t td = delta_time(start, end);
//    printf("time: %f\n", td.time_ms);

    // Turn P4.1 into an oscillator
    CAPTIO0CTL = 0; // Clear control register
    CAPTIO0CTL |= 0x0100; // Enable CAPTIO
    CAPTIO0CTL |= 4 << 4; // Select Port 4. Place port number in Bits 7-4
    CAPTIO0CTL |= 1 << 1; // Select Pin 1. Place pin number in Bits 3-1

    if ((CAPTIO0CTL & 1 << 8) == 0) {
        printf("CAPTIO0 is disabled, something is wrong.\n");
        while(1);
    }

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    // Init Timer_A in continuous mode
    Timer_A_ContinuousModeConfig timer_continuous_obj;
    timer_continuous_obj.clockSource = TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK;
    timer_continuous_obj.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_continuous_obj.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    timer_continuous_obj.timerClear = TIMER_A_DO_CLEAR;
    MAP_Timer_A_configureContinuousMode(TIMER_A2_BASE, &timer_continuous_obj);
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);



    // Print state endlessly
//    while(1) {
//        printf("%d", CAP_STATE);
//        fflush(stdout);
//    }


    // Measure timer A freq for 3ms
    //uint32_t start, end;
    uint64_t total_elapsed_ms = 0;
    uint32_t prev = CURRENT_TIME;
    uint32_t freq;

    while (total_elapsed_ms < 10000) {

        freq = measure_frequency();
        printf("Time: %llums, Frequency: %uHz\n", total_elapsed_ms, freq);

        uint32_t current = CURRENT_TIME;
        uint32_t tick_delta;

        if (prev >= current) {
            tick_delta = prev - current;
        } else {
            tick_delta = prev + ((uint32_t)0xFFFFFFFF - current);
        }

        uint32_t delta_millis = tick_delta * 1000 / MAP_CS_getMCLK();
        total_elapsed_ms += delta_millis;
        prev = current;
    }

    printf("Done\n");
    while(1);
}
