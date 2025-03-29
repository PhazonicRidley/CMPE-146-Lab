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
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

char* string1 = "xyz123";
char string2[] = "xyz123";
const char string3[1024 * 160] = {"xyz123"};

bool get_flash_bank_sector(uint32_t mem_address, uint32_t* bank_number, uint32_t* sector_mask)
{
    if(mem_address <= 0x0001FFFF)
    {
        *bank_number = FLASH_MAIN_MEMORY_SPACE_BANK0;    }
    else if (mem_address > 0x0001FFFF && mem_address <= 0x0003FFFF)
    {
        *bank_number =  FLASH_MAIN_MEMORY_SPACE_BANK1;
    }
    else
    {
        *bank_number = NULL;
    }
    *sector_mask = 1 << (mem_address/4096);
}

void main(void)
{
    uint32_t* bank1;
    uint32_t* mask1;
    uint32_t* bank3;
    uint32_t* mask3;

    MAP_WDT_A_holdTimer();
    printf("string1: %s\n", string1);
    printf("string2: %s\n", string2);
    printf("string2: %s\n", string3);

    string2[0] = '0';
    string2[1] = '3';

    get_flash_bank_sector((uint32_t)string1, bank1, mask1);
    get_flash_bank_sector((uint32_t)string3, bank3, mask3);

    FlashCtl_protectSector ( bank1, mask1);
    FlashCtl_protectSector ( bank1, mask1);

    ROM_FlashCtl_programMemory(string2, string1, 2);
    ROM_FlashCtl_programMemory(string2, string3, 2);

    printf("string1: %s\n", string1);
    printf("string2: %s\n", string2);
    printf("string2: %s\n", string3);
}

