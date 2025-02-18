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
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//![Simple GPIO Config]
#define RGBLED_PORT  GPIO_PORT_P2
#define RLED_PIN  GPIO_PIN0
#define GLED_PIN  GPIO_PIN1
#define BLED_PIN  GPIO_PIN2

#define BTN_PORT  GPIO_PORT_P1
#define BTN_PIN1  GPIO_PIN1
#define BTN_PIN4  GPIO_PIN4

int main(void)
{
    volatile uint32_t ii;
    MAP_WDT_A_holdTimer();

    //Initialize pins and ports for leds and buttons
    MAP_GPIO_setAsInputPinWithPullUpResistor(BTN_PORT, BTN_PIN1);

    MAP_GPIO_setAsOutputPin(RGBLED_PORT, RLED_PIN);
    MAP_GPIO_setAsOutputPin(RGBLED_PORT, GLED_PIN);
    MAP_GPIO_setAsOutputPin(RGBLED_PORT, BLED_PIN);

    // Bring LED to known state
    MAP_GPIO_setOutputLowOnPin(RGBLED_PORT, RLED_PIN);
    MAP_GPIO_setOutputLowOnPin(RGBLED_PORT, GLED_PIN);
    MAP_GPIO_setOutputLowOnPin(RGBLED_PORT, BLED_PIN);


    uint8_t currLed, prevLed = GPIO_INPUT_PIN_LOW;
    while(1)
    {
        currLed = MAP_GPIO_getInputPinValue(BTN_PORT, BTN_PIN1);
        for (ii = 0; ii < 8196; ii++); // Debounce S1
        if (currLed == GPIO_INPUT_PIN_LOW && currLed != prevLed) {
            printf("Pressed\n");
            MAP_GPIO_toggleOutputOnPin(RGBLED_PORT, BLED_PIN);
        }
        prevLed = currLed;
    }
}
