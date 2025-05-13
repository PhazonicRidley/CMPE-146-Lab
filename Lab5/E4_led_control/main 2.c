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

#define RGBLED_PORT  GPIO_PORT_P2
#define RLED_PIN  GPIO_PIN0

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


uint32_t reduce_sig_figs(uint32_t number) {
    if (number <= 0) return 0;

    int highest_mag = (int)log10(number);
    int mag = number > 100 ? highest_mag - 1: highest_mag;
    uint32_t divisor = (uint32_t)pow(10, mag);
    int msd = number / divisor;
    uint32_t result = msd * divisor;

    return result;
}


#define CAL_DAT_SIZE 500

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

    // Init Timer_A in continuous mode
    Timer_A_ContinuousModeConfig timer_continuous_obj;
    timer_continuous_obj.clockSource = TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK;
    timer_continuous_obj.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer_continuous_obj.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    timer_continuous_obj.timerClear = TIMER_A_DO_CLEAR;
    MAP_Timer_A_configureContinuousMode(TIMER_A2_BASE, &timer_continuous_obj);
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);

    MAP_GPIO_setAsOutputPin(RGBLED_PORT, RLED_PIN);
    MAP_GPIO_setOutputLowOnPin(RGBLED_PORT, RLED_PIN);


    uint32_t threshold;

    // Calibrate threshold
    printf("Calibrating, do not touch the pin\n");
    uint32_t calibration_data[CAL_DAT_SIZE];
    int i = 0;
    for (; i < CAL_DAT_SIZE; i++) {
        calibration_data[i] = measure_frequency();
    }

    // Take average for threshold
    uint64_t sum = 0;
    for (i = 0; i < CAL_DAT_SIZE; i++) {
        sum += (uint64_t)calibration_data[i];
    }

    threshold = reduce_sig_figs(sum / CAL_DAT_SIZE);

    printf("Done calibrating!\n");

    bool pressed = 0;
    bool prev_pressed = 0;
    while(1) {

        uint32_t cur_freq = measure_frequency();

        if (cur_freq < threshold) {
            pressed = 1;
        } else {
            pressed = 0;
        }

        if (pressed && pressed != prev_pressed) {
            MAP_GPIO_toggleOutputOnPin(RGBLED_PORT, RLED_PIN);
        }

        prev_pressed = pressed;

    }
}
