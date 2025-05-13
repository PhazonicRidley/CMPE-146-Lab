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
/*******************************************************************************
 * MSP432 GPIO - Toggle Output High/Low
 *
 * Description: In this very simple example, the LED on P1.0 is configured as
 * an output using DriverLib's GPIO APIs. An infinite loop is then started
 * which will continuously toggle the GPIO and effectively blink the LED.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.0  |---> P1.0 LED
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 ******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define DATA_LEN 10240

static uint8_t myData[DATA_LEN];
static uint8_t test_data[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint32_t compute_simple_checksum(uint8_t* data, uint32_t length) {
    uint32_t sum = 0;
    uint8_t* sum_bytes = (uint8_t*)&sum; // byte array of 4
    uint8_t byte_pos = 0;
    uint16_t buf;
    int i;
    for (i = 0; i < length; i++) {
        buf = data[i] + sum_bytes[byte_pos];
        sum_bytes[byte_pos] = buf & 0xFF;
        if ((buf & (1 << 8)) != 0 && byte_pos < 3) {

            int j = byte_pos + 1;
            buf = sum_bytes[j] + 1;
            sum_bytes[j] = buf & 0xFF;

            // Propagate carry if needed, loops at most thrice
            for (j++; j < 4; j++) {
                if ((buf & (1 << 8)) == 0) {
                    break;
                }
                buf = sum_bytes[j] + 1;
                sum_bytes[j] = buf & 0xFF;

            }
        }
        byte_pos = (byte_pos + 1) % 4;
    }

    return ~sum;
}

int main(void)
    {
    uint32_t res = compute_simple_checksum(myData, DATA_LEN);
    int i;
    printf("Dat first 4 bytes:\n");
    for (i = 0; i < 4; i++) {
        printf("0x%x\n", myData[i]);
    }
    printf("\nDat last 4 bytes:\n");
    for (i = DATA_LEN - 4; i < DATA_LEN; i++) {
        printf("0x%x\n", myData[i]);
    }
    printf("\nu32 checksum: %#010x\nInverted: %#010x\n\n", res, ~res);
    while(1);
}

//![Simple GPIO Config]


