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

void delay_ms(uint32_t count)
{
    uint64_t cc = MAP_CS_getMCLK();
    uint64_t end_tick = (cc * count) / 1000;

    uint32_t start = MAP_Timer32_getValue();
    uint32_t end = MAP_Timer32_getValue();

    while((start - end) <= end_tick)
    {
        end = MAP_Timer32_getValue();
    }
}


int main(void)
{
    uint32_t t0, t1;
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    MAP_GPIO_setAsOutputPin(RGBLED_PORT, RLED_PIN);

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    while(1)
    {
        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        delay_ms(25);
        MAP_GPIO_setOutputHighOnPin(RGBLED_PORT, RLED_PIN);
        delay_ms(2475);
        MAP_GPIO_setOutputLowOnPin(RGBLED_PORT, RLED_PIN);
        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        printf("Delay time: %d\n", (t1-t0));

        printf("===================================================");
    }

}
