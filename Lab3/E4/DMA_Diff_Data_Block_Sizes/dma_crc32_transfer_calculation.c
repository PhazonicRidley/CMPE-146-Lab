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
 * MSP432 DMA - CRC32 calculation using DMA
 *
 * Description: This code examples shows how to use the DMA module on MSP432 
 * to feed data into the CRC32 module for a CRC32 signature calculation. This
 * use case is particularly useful when the user wants to calculate the CRC32
 * signature of a large data array (such as a firmware image) but still wants
 * to maximize power consumption. After the DMA transfer is setup, a software
 * initiation occurs and the MSP432 device is put to sleep. Once the transfer
 * completes, the DMA interrupt occurs and the CRC32 result is placed into
 * a local variable for the user to examine.
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
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define CRC32_SEED 0xFFFFFFFF
#define DATA_LEN 1024

/* Statics */
static volatile uint32_t crcSignature;
volatile int dma_done;
volatile uint32_t hwCalculatedCRC, dmaCalculatedCRC;
int size_array[] = {2, 4, 16, 32, 64, 128, 256, 786, 1024};

uint32_t HW_t0;
uint32_t HW_t1;
uint32_t DMA_t0;
uint32_t DMA_t1;

/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(controlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif
uint8_t controlTable[1024];

/* Extern */
uint8_t data_array[1024];

int main(void)
{
    /* Halting Watchdog */
    MAP_WDT_A_holdTimer();

    //Set bit timer
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());
    HW_t0 = MAP_Timer32_getValue(TIMER32_0_BASE);

    /* Configuring DMA module */
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(controlTable);

    uint32_t jj;
    for(jj = 0; jj < 9; jj++)
    {
        int size = size_array[jj];
        printf("Array Size: %d\n", size);
        //hardware method;
        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);

        uint32_t ii;
        for (ii = 0; ii < size; ii++)
        {
            MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
        }

        hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
        HW_t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
        printf("Hardware method is done at time: %d us\n", HW_t0 - HW_t1);
        printf("Hardware method checksum: %d\n", hwCalculatedCRC);

        //dma method
        /* Setting Control Indexes. In this case we will set the source of the
         * DMA transfer to our random data array and the destination to the
         * CRC32 data in register address*/
        MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
                                  UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
        MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                                   UDMA_MODE_AUTO, data_array,
                                   (void*) (&CRC32->DI32), size);

        /* Assigning/Enabling Interrupts */
        MAP_DMA_assignInterrupt(DMA_INT1, 0);
        MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
        MAP_Interrupt_enableMaster();

        /* Enabling DMA Channel 0 */
        MAP_DMA_enableChannel(0);

        /* Forcing a software transfer on DMA Channel 0 */
        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
        MAP_DMA_requestSoftwareTransfer(0);


        dma_done = 0;

        while(1)
        {
            if(dma_done == 1)
            {
                DMA_t1 = MAP_Timer32_getValue(TIMER32_0_BASE);
                printf("DMA method Done at time: %d us\n", DMA_t0 - DMA_t1);
                printf("DMA method checksum: 0x%x\n", dmaCalculatedCRC);
                __delay_cycles(200 * 3000); // Delay 200 ms at 3 MHz
                break;
            }
        }

        printf("Speedup: %d us\n", (HW_t0 - HW_t1) / (DMA_t0 - DMA_t1));
        printf("------------------------------------------------------\n");
    }
}

/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_disableChannel(0);
    crcSignature = MAP_CRC32_getResultReversed(CRC32_MODE);
    dmaCalculatedCRC = crcSignature ^ 0xFFFFFFFF;
    DMA_t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    dma_done = 1;
}
