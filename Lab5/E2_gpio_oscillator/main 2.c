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
#define DATA_SIZE 100

static bool osc_data[DATA_SIZE];

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

int main(void)
{
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

    // Print state endlessly
//    while(1) {
//        printf("%d", CAP_STATE);
//        fflush(stdout);
//    }

    // Collect data
    uint32_t start = MAP_Timer32_getValue(TIMER32_0_BASE);
    int i = 0;
    for (; i < DATA_SIZE; i++) {
        osc_data[i] = CAP_STATE;
    }
    uint32_t end = MAP_Timer32_getValue(TIMER32_0_BASE);
    TimeData_t td = delta_time(start, end);

    // Print data
    for (i = 0; i < DATA_SIZE; i++) {
        printf("%d", osc_data[i]);
        fflush(stdout);
    }
    printf("\nTime taken to collect data: %fms\nSampling Rate for single measurement: %fkHz\n", td.time_ms, td.freq_khz * DATA_SIZE);
    printf("Done!\n");

    while(1);
}
