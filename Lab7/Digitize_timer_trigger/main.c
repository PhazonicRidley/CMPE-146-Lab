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
#include <stdio.h>
#include <string.h>

#define SYSTEM_FREQ 3000000
#define SAMPLING_FREQUENCY 100 // Sampling frequency in Hz
#define RESOLUTION 16384

volatile bool adc_done;
volatile uint32_t adc_value;
volatile float voltage;
volatile uint32_t start;
volatile uint32_t end;
volatile uint32_t tick_dur;
volatile int sample_counter = 0;

/* Timer_A Continuous Mode Configuration Parameter */
Timer_A_UpModeConfig upModeConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,            // ACLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,       // ACLK/1 = 32Khz
        16384,
        TIMER_A_TAIE_INTERRUPT_DISABLE,      // Disable Timer ISR
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // Disable CCR0
        TIMER_A_DO_CLEAR                     // Clear Counter
};

/* Timer_A Compare Configuration Parameter */
Timer_A_CompareModeConfig compareConfig =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_SET_RESET,               // Toggle output but
        16384                                       // 16000 Period
};

int main(void)
{
    // Halt WDT
    WDT_A_holdTimer();
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_FREE_RUN_MODE);
    Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("\nclock values: %u\n", CS_getSMCLK());

    // Set reference voltage to 1.2 V and enable temperature sensor
    REF_A_enableReferenceVoltage(); // enables REF_A to be used
    REF_A_enableTempSensor(); // enables temp sensor to be used
    REF_A_setReferenceVoltage(REF_A_VREF1_2V); // selects 1.2V reference voltage to be used

    // Initializing ADC (MCLK/1/1) with temperature sensor routed
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, ADC_TEMPSENSEMAP);

    // Set resolution
    ADC14_setResolution(ADC_14BIT);

    // Configure ADC Memory for temperature sensor data
    ADC14_configureSingleSampleMode(ADC_MEM0, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A22, false);

    //enabling configs for the Sample Frequency
    upModeConfig.timerPeriod = samFreqDiff;
    Timer_A_configureUpMode(TIMER_A0_BASE, &upModeConfig);
    compareConfig.compareValue = samFreqDiff / 2;

    //set up trigger from Timer_A
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig);
    ADC14_setSampleHoldTrigger(ADC_TRIGGER_SOURCE1, false);
    ADC14_enableConversion();

    //Enabling the interrupts
    ADC14_enableInterrupt(ADC_INT0);
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    //Measuring the Sampling Frequency
    uint32_t samTime = 1000 /SAMPLING_FREQUENCY;
    uint64_t samFreqDiff = ((uint64_t)samTime * CS_getSMCLK())/1000;
    printf("Sampling Freqency: %dHz | time difference: %llu\n", SAMPLING_FREQUENCY, samFreqDiff);

    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
    start = Timer32_getValue(TIMER32_0_BASE);
    while(sample_counter < 10){}
    end = Timer32_getValue(TIMER32_0_BASE);
    tick_dur = start - end;
    double seconds = (1.0 / CS_getMCLK()) * tick_dur;
    double testSamFreq = 10.0 / seconds;
    printf("measured time: %.3fs | calculated sampling freq: %.3fHz\n", seconds, testSamFreq);

    while (1)
    {
        adc_value = ADC14_getResult(ADC_MEM0);
        voltage = (adc_value * 1.2f) / RESOLUTION;
        printf("ADC: %u | Voltage: %.2f V\n", adc_value, voltage);
    }
}

// ADC ISR
void ADC14_IRQHandler(void)
{
    uint64_t status;
    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);
    if(status & ADC_INT0)
    {
        adc_value = ADC14_getResult(ADC_MEM0);
    }
    sample_counter++;
}


