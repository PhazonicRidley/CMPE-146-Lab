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

#define BAUD_RATE 1200
#define SYSTEM_FREQ 3000000
#define BIT_DURATION (SYSTEM_FREQ / BAUD_RATE)
#define UART_PORT GPIO_PORT_P1
#define UART_TX GPIO_PIN3

void send_char(char letter)
{
    int i;
    const uint32_t offset = 0;

    //start bit
    GPIO_setOutputLowOnPin(UART_PORT, UART_TX);
    __delay_cycles(BIT_DURATION - offset);
    //data bits
    for(i = 0; i < 8; i++)
    {
        if (letter & (1 << i))
        {
            GPIO_setOutputHighOnPin(UART_PORT, UART_TX);
        }
        else
        {
            GPIO_setOutputLowOnPin(UART_PORT, UART_TX);
        }
        __delay_cycles(BIT_DURATION - offset);
    }
    //stop bit
    GPIO_setOutputHighOnPin(UART_PORT, UART_TX);
    __delay_cycles(BIT_DURATION - offset);
}

void send_message(char* msg)
{
    int j = 0;
    while(msg[j] != '\0')
    {
        send_char(msg[j]);
        j++;
    }
}

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    GPIO_setAsOutputPin( UART_PORT, UART_TX);
    GPIO_setOutputHighOnPin(UART_PORT, UART_TX);
    __delay_cycles(3000);

    static char something[] =
    {0x0d,0x0a,0x4d,0x6f,0x6f,0x6f,0x6f,0x6f,0x20,0x20,0x20,0x20,0x20,0x20,0x5e,
     0x5f,0x5f,0x5e,0x0d,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x28,0x6f,0x6f,0x29,0x5c,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x5f,0x0d,0x0a,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x28,0x5f,0x5f,0x29,
    0x5c,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x29,0x5c,0x2f,0x5c,0x0d,0x0a,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x7c,0x7c,
    0x2d,0x2d,0x2d,0x2d,0x77,0x20,0x7c,0x0d,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x7c,0x7c,0x20,0x20,0x20,0x20,
    0x20,0x7c,0x7c,0x0d,0x0a,0x00};
    //static char something[] = {"Hello World"};
    send_message(something);
    while(1)
    {
        
    }
}
