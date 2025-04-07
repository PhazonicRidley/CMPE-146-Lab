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
#define RLED_PIN (0x0001)
#define GLED_PIN (0x0002)

uint32_t counter* = 0xFFFFFFF;

bool get_flash_bank_sector(uint32_t mem_address, uint32_t* bank_number, uint32_t* sector_mask)
{
    bool isMask = true;
    if(mem_address <= 0x0001FFFF)
    {
        *bank_number = FLASH_MAIN_MEMORY_SPACE_BANK0;
    }
    else if (mem_address > 0x0001FFFF && mem_address <= 0x0003FFFF)
    {
        *bank_number =  FLASH_MAIN_MEMORY_SPACE_BANK1;
    }
    else
    {
        *bank_number = NULL;
        isMask = false;
    }
    *sector_mask = 1 << (mem_address/4096);
    return isMask;
}


void gpio_toggle(volatile uint8_t* counter) {
  if (*counter < 3) {
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GLED_PIN);
    int i;
    while (1) {
      for(i = 0; i < 50000; i++);
      MAP_GPIO_toggleOutputPin(GPIO_PORT_P2, GLED_PIN);
    }
  }
  else {
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, RLED_PIN);
    int i;
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, RLED_PIN);
    while (1);
  }
}


void main(void)
{
    uint32_t current_counter;
    uint32_t mask;
    uint32_t bank;

    MAP_WDT_A_holdTimer();

    bool bank_sector = get_flash_bank_sector((uint32_t)string1, bank1, mask1);
    if(!bank_sector)
    {
        printf("Bank error");
    }

    FlashCtl_protectSector ( bank, mask);
    uint32_t next_counter = counter &(counter - 1;
    ROM_FlashCtl_programMemory(counter, next_counter, 2);
    gpio_toggle(counter);

}

