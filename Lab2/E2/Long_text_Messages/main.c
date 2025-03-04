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
    size_t delta, next_cursor, cursor = 0;
    do {
        uint8_t buf[16] = {0};
        const uint8_t* chunk = (const uint8_t*)str + cursor;
        delta = len - cursor;
        next_cursor = delta > 16 ? 16 : delta;
        memcpy(buf, chunk, next_cursor);
        MAP_AES256_startEncryptData(AES256_BASE, buf);

        while(MAP_AES256_isBusy(AES256_BASE)) {
            printf("Encrypting a string...\n");
        }

        MAP_AES256_getDataOut(AES256_BASE, encrypted + cursor);
        cursor += next_cursor;
    } while (cursor < len);
}

void decrypt_message(uint8_t* data, int data_length, uint8_t* decrypted, const char* key){
    char fixedKey[32];
    strncpy(fixedKey, key, 31);
    fixedKey[31] = '\0';
    MAP_AES256_setDecipherKey(AES256_BASE, (uint8_t*)fixedKey, AES256_KEYLENGTH_256BIT);
    memset(decrypted, 0, data_length);
    size_t delta, next_cursor, cursor = 0;
    do {
        uint8_t buf[16] = {0};
        delta = data_length - cursor;
        next_cursor = delta > 16 ? 16 : delta;
        const uint8_t* chunk =  data + cursor;
        memcpy(buf, chunk, next_cursor);
        MAP_AES256_startDecryptData(AES256_BASE, buf);

        while(MAP_AES256_isBusy(AES256_BASE)) {
            printf("Decrypting a string...\n");
        }

        MAP_AES256_getDataOut(AES256_BASE, decrypted + cursor);
        cursor += next_cursor;
    } while(cursor < data_length);
}

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();
    const char* zero_thru_j = "0123456789ABCDEFGHIJ";
    size_t len = strlen(zero_thru_j);
    encrypt_message(zero_thru_j, DataAESencrypted, str_key);
    decrypt_message(DataAESencrypted, len, DataAESdecrypted, str_key);

    printf("%s:", zero_thru_j);
    int i;
    printf("\nData: ");
    for (i = 0; i < len; i++) {
         printf("0x%02x ", zero_thru_j[i] );
    }

    int j;
    printf("\nEncrypted: ");
    for (j = 0; j < len; j++) {
         printf("0x%02x ", DataAESencrypted[j] );
    }

    int k;
    printf("\nDencrypted: ");
    for (k = 0; k < len; k++) {
         printf("0x%02x ", DataAESdecrypted[k] );
    }

    const char* one_thru_five = "12345";
    len = strlen(one_thru_five);
    encrypt_message(one_thru_five, DataAESencrypted, str_key);
    decrypt_message(DataAESencrypted, len, DataAESdecrypted, str_key);

    printf("\n\n%s:", one_thru_five);
    printf("\nData: ");
    for (i = 0; i < len; i++) {
         printf("0x%02x ", one_thru_five[i]);
    }

    printf("\nEncrypted: ");
    for (j = 0; j < len; j++) {
         printf("0x%02x ", DataAESencrypted[j] );
    }

    printf("\nDencrypted: ");
    for (k = 0; k < len; k++) {
         printf("0x%02x ", DataAESdecrypted[k] );
    }
    printf("\n\n");


    const char* zero_thru_f = "0123456789ABCDEF";
    len = strlen(zero_thru_f);
    encrypt_message(zero_thru_f, DataAESencrypted, str_key);
    decrypt_message(DataAESencrypted, len, DataAESdecrypted, str_key);

    printf("\n\n%s:", zero_thru_f);
    printf("\nData: ");
    for (i = 0; i < len; i++) {
         printf("0x%02x ", zero_thru_f[i]);
    }

    printf("\nEncrypted: ");
    for (j = 0; j < len; j++) {
         printf("0x%02x ", DataAESencrypted[j] );
    }

    printf("\nDencrypted: ");
    for (k = 0; k < len; k++) {
         printf("0x%02x ", DataAESdecrypted[k] );
    }
    printf("\n\n");

    const char* fox_dog = "The quick brown fox jumps over the lazy dog.";
    len = strlen(fox_dog);
    encrypt_message(fox_dog, DataAESencrypted, str_key);
    decrypt_message(DataAESencrypted, len, DataAESdecrypted, str_key);

    printf("\n\n%s:", fox_dog);
    printf("\nData: ");
    for (i = 0; i < len; i++) {
         printf("0x%02x ", fox_dog[i]);
    }

    printf("\nEncrypted: ");
    for (j = 0; j < len; j++) {
         printf("0x%02x ", DataAESencrypted[j] );
    }

    printf("\nDencrypted: ");
    for (k = 0; k < len; k++) {
         printf("0x%02x ", DataAESdecrypted[k] );
    }
    printf("\n\n");

    while(1)
    {
        MAP_PCM_gotoLPM0();

    }
}
