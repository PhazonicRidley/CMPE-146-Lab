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

//Calculates a Fletcher-32 checksum, which consists of two parts.
//The first is a checksum of the data values. The second
//is a checksum of the intermediate values of the first checksum.
uint32_t Calc_Fletcher32_Chksum(uint16_t *data_start, uint32_t num_data_values, uint32_t seed)
{
    uint32_t sum1, sum2, c;
    sum1 = seed & 0xFFFF;
    sum2 = (seed & 0xFFFF0000) >> 16;
    for (c = 0; c < num_data_values; c++)
    {
            //Add the new data value, doing an end-around carry to implement a
            //one's complement sum. This is mostly done for better error detection
            //since it makes an MSB flip affect more than just the MSB of the
            //checksum. It's also endian-independent (up to a bit rotation) and
            //provides easy match detection, although those features don't work in
            //a Fletcher checksum. This step can be optimized for speed by
            //accumulating carries for up to 360 sums, but since the function is
            //only used once per run the code size is optimized instead.
            sum1 += data_start[c];
            sum1 = (sum1 >> 16) + (sum1 & 0xFFFF);
            sum2 += sum1;
            sum2 = (sum2 >> 16) + (sum2 & 0xFFFF);
    }
    return (sum2<<16) | sum1;
}


int main(void)
{
    uint16_t *tlv_data =(uint16_t *) 0x00201004;
    uint32_t chksumSeed = 0xDADA0000;
    uint32_t computed_checksum = Calc_Fletcher32_Chksum(tlv_data, 2, chksumSeed);

    printf("Computed Checksum: 0x%08X\n", computed_checksum);

}
