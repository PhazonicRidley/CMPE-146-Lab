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

#define USE_SRAM_METHOD
#define USE_FLASH_METHOD
#define DO_PRINTF


#ifdef USE_SRAM_METHOD
#pragma CODE_SECTION(sram_function, ".TI.ramfunc")
void sram_function()
{
    register int i;
    register int count = 20000000;
    for (i=0; i<count; i++);
}
#endif


#ifdef USE_SRAM_METHOD
void flash_function()
{
    register int i;
    register int count = 20000000;
    for (i=0; i<count; i++);
}
#endif

#ifdef DO_PRINTF
uint64_t delta_micros(uint32_t start, uint32_t end) {
    uint64_t tick_delta = abs(start - end); // Countdown timer so start is a greater value than end
    uint64_t time = tick_delta / MAP_CS_getMCLK();
    uint64_t time_micros = time * 1000000;
    return time_micros;

}
#endif


int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

#ifdef DO_PRINTF
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    uint32_t start, end;
#endif

    // Test flash memory
#ifdef USE_FLASH_METHOD
    FlashCtl_disableReadBuffering(FLASH_BANK0, FLASH_INSTRUCTION_FETCH);
    FlashCtl_disableReadBuffering(FLASH_BANK1, FLASH_INSTRUCTION_FETCH);
#ifdef DO_PRINTF
    start = MAP_Timer32_getValue(TIMER32_0_BASE);
#endif

    flash_function();

#ifdef DO_PRINTF
    end = MAP_Timer32_getValue(TIMER32_0_BASE);

    printf("Flash function being measured, fn addr: %p, clock frequency is: %d, function runtime: %llu\n",
           &flash_function, MAP_CS_getMCLK(), delta_micros(start, end));
#endif

#endif

    // Test SRAM

#ifdef USE_SRAM_METHOD

#ifdef DO_PRINTF
    start = MAP_Timer32_getValue(TIMER32_0_BASE);
#endif

    sram_function();

#ifdef DO_PRINTF
    end = MAP_Timer32_getValue(TIMER32_0_BASE);

    printf("SRAM function being measured, fn addr: %p, clock frequency is: %d, function runtime: %llu\n",
           &sram_function, MAP_CS_getMCLK(), delta_micros(start, end));
#endif

#endif


#ifdef DO_PRINTF
    printf("Done!");
#endif

    while(1);

}
