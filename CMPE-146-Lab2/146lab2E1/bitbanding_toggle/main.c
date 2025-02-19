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

// Base addresses for bit-banding calculation
#define PERIPHERAL_REGION_ADDR 0x40000000    // Starting address of peripheral region
#define ALIAS_REGION_ADDR     0x42000000    // Starting address of alias region

//![Simple GPIO Config]
#define RGBLED_PORT  GPIO_PORT_P2
#define RLED_PIN  GPIO_PIN0
#define GLED_PIN  GPIO_PIN1
#define BLED_PIN  GPIO_PIN2

#define BTN_PORT  GPIO_PORT_P1
#define BTN_PIN1  GPIO_PIN1

#define PORT_REG(offset) ((volatile uint8_t*)(0x40004C00 + offset))

#define BITBAND_ADDR(port_addr, bit_position) \
    ((volatile uint8_t*)(( (uint32_t)(port_addr) - PERIPHERAL_REGION_ADDR) * 32 + ( (uint32_t)bit_position * 4) + ALIAS_REGION_ADDR))

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


    volatile uint8_t* btn_switch_bit_alias = BITBAND_ADDR(PORT_REG(0x0), 1);
    volatile uint8_t* blue_led_alias = BITBAND_ADDR(PORT_REG(0x3), 2);
    uint8_t btn_state, prev_btn_state = GPIO_INPUT_PIN_LOW;
    printf("Hello bitbanding\n");
    while(1)
    {
        btn_state = *btn_switch_bit_alias;
        printf("btn_switch_bit_alias: 0x%p, value: %d\n\n", btn_switch_bit_alias, btn_state);
        printf("blue led_alias: 0x%p, value: %d\n", blue_led_alias, *blue_led_alias);
        for (ii = 0; ii < 8196; ii++); // Debounce S1
        if (btn_state == GPIO_INPUT_PIN_LOW && prev_btn_state != btn_state) {
            printf("Pressed (bitband)\n");
            *blue_led_alias ^= 1;
        }
        prev_btn_state = btn_state;
    }
}
