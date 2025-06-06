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
 * MSP432 AES256 -  Encryption and Decryption Example
 *
 *  Description: This demo shows a simple example of encryption and decryption
 *  using the AES356 module.
 *
 *               MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 * Key: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f
 * Plaintext: 00112233445566778899aabbccddeeff
 * Ciphertext: 8ea2b7ca516745bfeafc49904b496089
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* Statics */

const static uint8_t CipherKey[32] =
{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
        0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
static uint8_t DataAESencrypted[16];       // Encrypted data
static uint8_t DataAESdecrypted[16];       // Decrypted data

void encrypt_message_16(const char* str, uint8_t* encrypted, const uint8_t* key)
{
    MAP_AES256_setCipherKey(AES256_BASE, key, AES256_KEYLENGTH_256BIT);

    MAP_AES256_startEncryptData(AES256_BASE, (const uint8_t*)str);

    while(MAP_AES256_isBusy(AES256_BASE)) {
        printf("Encrypting a string...\n");
    }

    MAP_AES256_getDataOut(AES256_BASE, encrypted);
}

void decrypt_message_16(uint8_t* data, uint8_t* decrypted, const uint8_t* key)
{
    MAP_AES256_setDecipherKey(AES256_BASE, key, AES256_KEYLENGTH_256BIT);

    MAP_AES256_startDecryptData(AES256_BASE, data);

    while(MAP_AES256_isBusy(AES256_BASE)) {
        printf("Encrypting a string...\n");
    }

    MAP_AES256_getDataOut(AES256_BASE, decrypted);
}

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    const char* full_data = "0123456789ABCDE";
    const char* truncated_data = "0123456789";
    encrypt_message_16(full_data, DataAESencrypted, CipherKey);
    decrypt_message_16(DataAESencrypted, DataAESdecrypted, CipherKey);

    printf("Full 16 byte string:");
    int i;
    printf("\nData: ");
    for (i = 0; i < 16; i++) {
         printf("0x%02x ", full_data[i] );
    }

    int j;
    printf("\nEncrypted: ");
    for (j = 0; j < 16; j++) {
         printf("0x%02x ", DataAESencrypted[j] );
    }

    int k;
    printf("\nDencrypted: ");
    for (k = 0; k < 16; k++) {
         printf("0x%02x ", DataAESdecrypted[k] );
    }

    encrypt_message_16(truncated_data, DataAESencrypted, CipherKey);
    decrypt_message_16(DataAESencrypted, DataAESdecrypted, CipherKey);
    printf("\n\nTruncated 10 byte string:");
    printf("\nData: ");
    for (i = 0; i < 16; i++) {
         printf("0x%02x ", truncated_data[i]);
    }

    printf("\nEncrypted: ");
    for (j = 0; j < 16; j++) {
         printf("0x%02x ", DataAESencrypted[j] );
    }

    printf("\nDencrypted: ");
    for (k = 0; k < 16; k++) {
         printf("0x%02x ", DataAESdecrypted[k] );
    }
    printf("\n\n");
    while(1)
    {
        MAP_PCM_gotoLPM0();

    }
}
