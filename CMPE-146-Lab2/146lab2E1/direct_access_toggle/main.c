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

//![Simple GPIO Config]
#define RLED_PIN (0x0001)
#define GLED_PIN (0x0002)
#define BLED_PIN (0x0004)

#define BTN_PIN1 (0x0002)

#define PORT_REG(offset) ((uint8_t*)(0x40004C00 + offset))

int main(void)
{
    /* Stop Watchdog  */
    printf("hello\n");
    // Configure button
    volatile uint8_t* btn_port_dir_reg = PORT_REG(0x4);
    volatile uint8_t* btn_port_ren_reg = PORT_REG(0x6);
    volatile uint8_t* btn_port_out_reg = PORT_REG(0x2);
    volatile uint8_t* btn_port_in_reg = PORT_REG(0x0);
    *btn_port_dir_reg &= ~(BTN_PIN1);
    *btn_port_ren_reg |= BTN_PIN1;
    *btn_port_out_reg |= BTN_PIN1;

    // Configure not LED
    volatile uint8_t* led_port_dir_reg = PORT_REG(0x5);
    volatile uint8_t* led_port_out_reg = PORT_REG(0x3);
    *led_port_dir_reg |= RLED_PIN | GLED_PIN | BLED_PIN;
    *led_port_out_reg &= ~(RLED_PIN | GLED_PIN | BLED_PIN);

    uint8_t btn_state, prev_btn_state = GPIO_INPUT_PIN_LOW;
    volatile int ii;

    while(1)
    {
        btn_state = *btn_port_in_reg & BTN_PIN1;
        for (ii = 0; ii < 8196; ii++); // Debounce S1
        if (btn_state == GPIO_INPUT_PIN_LOW && prev_btn_state != btn_state) {
            printf("Pressed\n");
            *led_port_out_reg ^= BLED_PIN;
        }
        prev_btn_state = btn_state;
    }
}
