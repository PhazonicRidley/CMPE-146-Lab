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

#pragma CODE_SECTION(sram_function, ".TI.ramfunc")
void sram_function()
{
    register int i;
    register int count = 100000;
    for (i=0; i<count; i++);
}


void flash_function()
{
    register int i;
    register int count = 100000;
    for (i=0; i<count; i++);
}

double delta_micros(uint32_t start, uint32_t end) {
    uint32_t tick_delta = abs(start - end); // Countdown timer so start is a greater value than end
    double time = (double)tick_delta / MAP_CS_getMCLK();
    double time_micros = time * pow(10, 6);
    return time_micros;

}


int main(void)
{
    FlashCtl_disableReadBuffering(FLASH_BANK0, FLASH_INSTRUCTION_FETCH);
    FlashCtl_disableReadBuffering(FLASH_BANK1, FLASH_INSTRUCTION_FETCH);

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    printf("Hello\n");
    uint32_t start, end;
    while(1)
    {
        printf("Clock Frequency %u\n", MAP_CS_getMCLK());
        start = MAP_Timer32_getValue(TIMER32_0_BASE);
        flash_function();
        end = MAP_Timer32_getValue(TIMER32_0_BASE);
        printf("Flash function (fn addr: %p) took: %fus\n", flash_function, delta_micros(start, end));

        printf("Clock Frequency %u\n", MAP_CS_getMCLK());
        start = MAP_Timer32_getValue(TIMER32_0_BASE);
        sram_function();
        end = MAP_Timer32_getValue(TIMER32_0_BASE);
        printf("SRAM function (fn addr: %p) took: %fus\n\n", sram_function, delta_micros(start, end));

        
    }
}
