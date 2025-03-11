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

/*Pin Config*/
#define RGBLED_PORT  GPIO_PORT_P2
#define GLED_PIN  GPIO_PIN1

#define BTN_PORT  GPIO_PORT_P1
#define BTN_PIN1  GPIO_PIN1

#define QUEUE_SIZE 10
struct record {
    uint32_t timestamp;
    uint8_t state;
};
struct record queue[QUEUE_SIZE];
volatile int read_index;
volatile int write_index;

volatile uint8_t currEdge = GPIO_HIGH_TO_LOW_TRANSITION;
volatile struct record head_queue;
volatile uint32_t start_time;
volatile uint32_t end_time;
volatile uint32_t time_dur;

//Circular queue
//isFull
bool isFull()
{
    if(((write_index + 1) % QUEUE_SIZE) == read_index)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//isEmpty
bool isEmpty()
{
    if(read_index == write_index)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//enqueue
void enqueue (uint32_t timestamp, uint8_t state){
    if(isFull())
    {
        printf("Full\n");
        return;
    }
    queue[write_index].timestamp = timestamp;
    queue[write_index].state = state;

    write_index = (write_index + 1) % QUEUE_SIZE;
}
//dequeue
struct record dequeue ()
{
    if(isEmpty())
    {
        printf("Empty\n");
        return;
    }

    struct record record_return = queue[read_index];
    read_index = (read_index + 1) % QUEUE_SIZE;

    return record_return;
}
//peek
struct record peek ()
{
    if(isEmpty())
    {
        printf("Empty\n");
        return;
    }

    return queue[read_index];
}

int main(void)
{
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    //set up inputs and outputs
    MAP_GPIO_setAsOutputPin(RGBLED_PORT, GLED_PIN);
    MAP_GPIO_setAsInputPinWithPullUpResistor(BTN_PORT, BTN_PIN1);

    //set up interrupts
    MAP_GPIO_clearInterruptFlag(BTN_PORT, BTN_PIN1);
    MAP_GPIO_interruptEdgeSelect(BTN_PORT, BTN_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    MAP_GPIO_enableInterrupt(BTN_PORT, BTN_PIN1);
    MAP_Interrupt_enableInterrupt(INT_PORT1);

    /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();

    //Set bit timer
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());


    //delay
    while (1)
    {
        __delay_cycles(200 * 3000); // Delay 200 ms at 3 MHz
        if (!isEmpty() && BTN_PIN1)
        {
            //deque from queue and check
            head_queue = dequeue();
            //printf("State: 0x%x, Time: 0x%x\n", head_queue.state, head_queue.timestamp);

            //if high->low, start time
            if(head_queue.state == GPIO_HIGH_TO_LOW_TRANSITION && BTN_PIN1)
            {
                printf("Button Pressed\n");
                printf("Read_index: %d \n", read_index);
                printf("---------------------------------------------\n");
                start_time = head_queue.timestamp;
            }
            //if low->high, end time
            else if(head_queue.state == GPIO_LOW_TO_HIGH_TRANSITION && BTN_PIN1)
            {
                printf("Button Released\n");
                end_time = head_queue.timestamp;
                time_dur = start_time - end_time;
                printf("Read_index: %d \n", read_index);
                printf("Button Time Duration: %d us\n", time_dur);
                printf("---------------------------------------------\n");
            }
        }
    }
}

/* GPIO ISR */
void PORT1_IRQHandler(void)
{
    uint32_t status;
    uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE);

    status = MAP_GPIO_getEnabledInterruptStatus(BTN_PORT);
    MAP_GPIO_clearInterruptFlag(BTN_PORT, status);

    /* Toggling the output on the LED */
    if(status & BTN_PIN1)
    {
        if(currEdge == GPIO_LOW_TO_HIGH_TRANSITION)
        {
            enqueue(t0, GPIO_LOW_TO_HIGH_TRANSITION);
            currEdge = GPIO_HIGH_TO_LOW_TRANSITION;
        }
        else
       {
            MAP_GPIO_toggleOutputOnPin(RGBLED_PORT, GLED_PIN);
            enqueue(t0, GPIO_HIGH_TO_LOW_TRANSITION);
            currEdge = GPIO_LOW_TO_HIGH_TRANSITION;
       }
       MAP_GPIO_interruptEdgeSelect(BTN_PORT, BTN_PIN1, currEdge);
    }
}
