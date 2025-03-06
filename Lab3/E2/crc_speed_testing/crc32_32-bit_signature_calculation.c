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
 * MSP432 CRC32 - CRC32 Calculation
 *
 * Description: In this example, the CRC32 module is used to calculate a CRC32
 * checksum in CRC32 mode. This value is compared versus a software calculated
 * checksum. The idea here is to have the user use the debugger to step through
 * the code and observe the CRC calculation in the debugger. Note that this
 * code example was made to use the standard CRC32 polynomial value of
 * 0xCBF43926.
 *
 *              MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 ******************************************************************************/
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define CRC32_POLY              0xEDB88320
#define CRC32_INIT              0xFFFFFFFF
#define DATA_LEN                10240

static const uint8_t myData[DATA_LEN]; //=  { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39 };

static uint32_t calculateCRC32(uint8_t* data, uint32_t length);
static uint32_t hwCRC32(uint8_t* data, uint32_t length);
uint32_t compute_simple_checksum(uint8_t* data, uint32_t length);

double micros(uint32_t tick_count) {
    double seconds = (double)tick_count / MAP_CS_getMCLK();
    return pow(seconds * 10, 6);
}

//![Simple CRC32 Example] 
int main(void)
{
    int i;
    printf("Dat first 4 bytes:\n");
    for (i = 0; i < 4; i++) {
        printf("0x%x\n", myData[i]);
    }
    printf("\nDat last 4 bytes:\n");
    for (i = DATA_LEN - 4; i < DATA_LEN; i++) {
        printf("0x%x\n", myData[i]);
    }
    printf("\n\n\n");


    volatile uint32_t hwCalculatedCRC, swCalculatedCRC;
    uint32_t hwTime_start, hwTime_end,
        swTime_start, swTime_end, checksumTime_start, checksumTime_end;

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("Clock Frequency %u\n", MAP_CS_getMCLK());
    /* Stop WDT */
    MAP_WDT_A_holdTimer();

    /* Getting the result from the hardware module */
    printf("HW CRC time: ");
    hwTime_start = MAP_Timer32_getValue(TIMER32_0_BASE);
    hwCalculatedCRC = hwCRC32((uint8_t*) myData, DATA_LEN);
    hwTime_end = MAP_Timer32_getValue(TIMER32_0_BASE);
    double hwTime = micros(abs(hwTime_start - hwTime_end));
    printf("%lfus\n", hwTime);

    /* Calculating the CRC32 checksum through software */
    printf("\nSW CRC time: ");
    swTime_start = MAP_Timer32_getValue(TIMER32_0_BASE);
    swCalculatedCRC = calculateCRC32((uint8_t*) myData, DATA_LEN);
    swTime_end = MAP_Timer32_getValue(TIMER32_0_BASE);
    double swTime = micros(abs(swTime_start - swTime_end));
    printf("%lfus\n", swTime);


    printf("\nSimple Checksum time: ");
    checksumTime_start = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t checksum = compute_simple_checksum((uint8_t*)myData, DATA_LEN);
    checksumTime_end = MAP_Timer32_getValue(TIMER32_0_BASE);
    double chkTime = micros(abs(checksumTime_start - checksumTime_end));
    printf("%lfus\n", chkTime);

    printf("hwCRC: %x, swCRC: %x, simple checksum: %x\n", hwCalculatedCRC, swCalculatedCRC, checksum);
    printf("CRCs equal?: %d\n", hwCalculatedCRC == swCalculatedCRC);

    /* Pause for the debugger */
    while(1);
}
//![Simple CRC32 Example] 

/* Standard software calculation of CRC32 */
static uint32_t calculateCRC32(uint8_t* data, uint32_t length)
{
    uint32_t ii, jj, byte, crc, mask;;

    crc = 0xFFFFFFFF;

    for(ii=0;ii<length;ii++)
    {
        byte = data[ii];
        crc = crc ^ byte;

        for (jj = 0; jj < 8; jj++)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (CRC32_POLY & mask);
        }

    }

    return ~crc;
}

static uint32_t hwCRC32(uint8_t* data, uint32_t length) {
    uint32_t ii;

    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);

    for (ii = 0; ii < DATA_LEN; ii++)
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

    /* Getting the result from the hardware module */
    return MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;

}

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
