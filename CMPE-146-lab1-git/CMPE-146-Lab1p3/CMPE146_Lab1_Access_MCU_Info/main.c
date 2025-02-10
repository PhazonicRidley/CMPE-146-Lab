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

// Define the memory addresses defined in datasheet (table 6-86)
#define TLV_CHECKSUM_ADDR (uint32_t*)0x00201000
#define TLV_ENDWORD_ADDR (uint32_t*)0x00201148

// Structure to represent a TLV entry
typedef struct tlv_entry_s {
    uint32_t tag; // Tag is the first 4 bytes
    uint32_t len; // length is the next 4 bytes
    uint32_t data[]; // Zero length array representing that there will be consecutive u32s after len
} tlv_entry_t;



// Print out the entire device descriptor table, categorized by entry
void print_device_descriptor_table() {
    // Print checksum
    uint32_t* checksum_addr = TLV_CHECKSUM_ADDR;
    printf("TLV Checksum: 0x%x\n", *checksum_addr);
    printf("-------------------\n");

    // Next 4 bytes over (0x00201004) starts the first entry, cast the new pointer to consider the underlying type to be
    // a tlv_entry_t
    tlv_entry_t* cur_addr = (tlv_entry_t*)(checksum_addr + 1); // + 1 means "the next u32" or "the next 4 bytes to get to the next u32"
    uint32_t entry_len;

    // While the current address is less than the endword address, we are still in the table
    while ((uint32_t*)cur_addr < TLV_ENDWORD_ADDR) {
        entry_len = cur_addr->len;
        printf("tlv_entry_addr: %p", cur_addr); // print start of current entry address
        printf("Tag: 0x%x\n", cur_addr->tag);
        printf("Length: 0x%x\nData:", entry_len);
        
        // Print data associated with the current entry, we know how much data there will be from len
        int i;
        for (i = 0; i < entry_len && (entry_len < 255); i++) {
            printf("%d: 0x%x\n", i, cur_addr->data[i]);
        }

        // TLV entry is variable length so incrementation must be done at the word level because thats a fixed size we can measure by
        uint32_t next_entry_offset = 2 + entry_len;
        cur_addr = (tlv_entry_t*)((uint32_t*)cur_addr + next_entry_offset);

        printf("-------------------\n");
    }

    // Print the endword, last part of the table.
    printf("Endword: 0x%x\n", *(uint32_t*)cur_addr);
}

int main(void)
{
    print_device_descriptor_table();
    printf("\nDone\n");
    while(1);

}
