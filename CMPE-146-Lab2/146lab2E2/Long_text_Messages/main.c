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


const char* str_key = "Password is CMPE146-01";
static uint8_t DataAESencrypted[50];
static uint8_t DataAESdecrypted[50];

void encrypt_message(const char* str, uint8_t* encrypted, const char* key) {
    char fixedKey[32];
    strncpy(fixedKey, key, 31);
    fixedKey[31] = '\0';
    MAP_AES256_setCipherKey(AES256_BASE, (uint8_t*)fixedKey, AES256_KEYLENGTH_256BIT);

    size_t len = strlen(str); // Assumes str is null terminated
    size_t delta, cursor = 0;
    do {

        const uint8_t* chunk = (const uint8_t*)str + cursor;
        MAP_AES256_startEncryptData(AES256_BASE, chunk);

        while(MAP_AES256_isBusy(AES256_BASE)) {
            printf("Encrypting a string...\n");
        }

        MAP_AES256_getDataOut(AES256_BASE, encrypted + cursor);
        delta = len - cursor;
        cursor += delta > 16 ? 16 : delta;
    } while (cursor < len);
}

void decrypt_message(uint8_t* data, int data_length, uint8_t* decrypted, const char* key){
    char fixedKey[32];
    strncpy(fixedKey, key, 31);
    fixedKey[31] = '\0';
    MAP_AES256_setDecipherKey(AES256_BASE, (uint8_t*)fixedKey, AES256_KEYLENGTH_256BIT);

    size_t delta, cursor = 0;
    do {
        MAP_AES256_startDecryptData(AES256_BASE, data + cursor);

        while(MAP_AES256_isBusy(AES256_BASE)) {
            printf("Decrypting a string...\n");
        }

        MAP_AES256_getDataOut(AES256_BASE, decrypted + cursor);
        delta = data_length - cursor;
        cursor += delta > 16 ? 16 : cursor;
    } while(cursor < data_length);
}

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    const char* long_data = "0123456789ABCDEFGHIJ";
    size_t long_data_len = strlen(long_data);
    encrypt_message(long_data, DataAESencrypted, str_key);
    decrypt_message(DataAESencrypted, long_data_len, DataAESdecrypted, str_key);

    int i;
    printf("\nData: ");
    for (i = 0; i < long_data_len; i++) {
         printf("0x%02x ", long_data[i] );
    }

    int j;
    printf("\nEncrypted: ");
    for (j = 0; j < long_data_len; j++) {
         printf("0x%02x ", DataAESencrypted[j] );
    }

    int k;
    printf("\nDencrypted: ");
    for (k = 0; k < long_data_len; k++) {
         printf("0x%02x ", DataAESdecrypted[k] );
    }

    printf("\n");
    while(1)
    {
        MAP_PCM_gotoLPM0();

    }
}
