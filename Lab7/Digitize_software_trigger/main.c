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

// Standard Includes
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SYSTEM_FREQ 3000000
#define res 16384

volatile bool adc_done;
volatile uint32_t adc_value;
volatile float voltage;
volatile float temp_c;

int main(void)
{
    // Halt WDT
    WDT_A_holdTimer();
    printf("MCLK: %u\n", MAP_CS_getMCLK());

    // Set reference voltage to 1.2 V and enable temperature sensor
    REF_A_enableReferenceVoltage();
    REF_A_enableTempSensor();
    REF_A_setReferenceVoltage(REF_A_VREF1_2V);

    // Initializing ADC (MCLK/1/1) with temperature sensor routed
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1,
                     ADC_TEMPSENSEMAP);

    // Set resolution
    ADC14_setResolution(ADC_14BIT);

    // Configure ADC Memory for temperature sensor data
    ADC14_configureSingleSampleMode(ADC_MEM0, false);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS,
                                    ADC_INPUT_A22, false);

    // Configure the sample/hold time
    ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_192, ADC_PULSE_WIDTH_192);

    // Enable sample timer in manual iteration mode and interrupts
    ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    // Enable conversion
    ADC14_enableConversion();

    // Enabling Interrupts
    ADC14_enableInterrupt(ADC_INT0);
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    // Trigger conversion with software
    ADC14_toggleConversionTrigger();
    while (true)
    {
        adc_value = ADC14_getResult(ADC_MEM0);
        voltage = (adc_value * 1.2f) / res;
        temp_c = voltage * 100;
        printf("voltage: %.2f | ADC: %d\n", voltage, adc_value);
        __delay_cycles((SYSTEM_FREQ / 1000) * 500);
    }
}

// This interrupt happens every time a conversion has completed
void ADC14_IRQHandler(void)
{
    uint64_t status;
    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);
    if(status & ADC_INT0)
    {
            adc_done = true;
    }
}


