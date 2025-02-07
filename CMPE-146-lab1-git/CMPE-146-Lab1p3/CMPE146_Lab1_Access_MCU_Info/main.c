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

#define TLV_CHECKSUM_ADDR (void*)0x00201000
#define TLV_ENDWORD_ADDR (void*)0x00201148


typedef struct tlv_entry_s {
    uint32_t tag;
    uint32_t len;
    uint32_t data[];
} tlv_entry_t;




void print_device_descriptor_table() {
    uint32_t* checksum_addr = TLV_CHECKSUM_ADDR;
    printf("TLV Checksum: 0x%x\n", *checksum_addr);
    printf("-------------------\n");
    tlv_entry_t* cur_addr = (tlv_entry_t*)(checksum_addr + 1);
    uint32_t entry_len;
    while ((void*)cur_addr < TLV_ENDWORD_ADDR) {
        entry_len = cur_addr->len;
        printf("tlv_entry_addr: %p\n", cur_addr);
        printf("Tag: 0x%x\n", cur_addr->tag);
        printf("Length: 0x%x\nData:", entry_len);

        int i;
        for (i = 0; i < entry_len && (entry_len < 255); i++) {
            printf("%d: 0x%x\n", i, cur_addr->data[i]);
        }

        // TLV entry is variable length so incrementation must be done at the word level
        uint32_t next_addr = 2 + entry_len;
        cur_addr = (tlv_entry_t*)((uint32_t*)cur_addr + next_addr);

        printf("-------------------\n");
    }
    printf("Endword: 0x%x\n", *(uint32_t*)cur_addr);
}

int main(void)
{
    print_device_descriptor_table();
    printf("\nDone\n");
    while(1);

}
