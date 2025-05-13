/*******************************************************************************
* Program to blink the red LED with push button S1 in LPM4.5.
******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//#define USE_POLLING
volatile int button_pressed = 0;

// Set up GPIO ports for maximizing power saving
void setup_all_GPIO_ports(bool set_state)
{
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PA, PIN_ALL16);   // Ports 1 and 2
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PB, PIN_ALL16);   // Ports 3 and 4
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PC, PIN_ALL16);   // Ports 5 and 6
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PD, PIN_ALL16);   // Ports 7 and 8
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PE, PIN_ALL16);   // Ports 9 and 10
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PJ, PIN_ALL16);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, PIN_ALL8);

    if (set_state)      // Do not set to known states if returning from LPM. Need to preserve them
    {
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PA, PIN_ALL16);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PB, PIN_ALL16);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PC, PIN_ALL16);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PD, PIN_ALL16);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PE, PIN_ALL16);
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PJ, PIN_ALL16);
    }
}

// Blink briefly an LED
void blink_LED(uint8_t port, uint8_t pin)
{
    MAP_GPIO_setOutputHighOnPin(port, pin);
    __delay_cycles(10 * 3000);      // 10 ms at 3 MHz
    MAP_GPIO_setOutputLowOnPin(port, pin);
}

// Set up for push button interrupt
void setup_push_button()
{
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
#ifdef USE_POLLING
    return;
#else
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    MAP_Interrupt_enableInterrupt(INT_PORT1);
#endif
}

int main(void)
{
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    if (MAP_ResetCtl_getPCMSource() & RESET_LPM45)      // Wake up from LPM4.5
    {
        MAP_ResetCtl_clearPCMFlags();        // Clearing the PCM Reset flags
        PCM->CTL1 = PCM_CTL1_KEY_VAL;        // Unlocking the latched GPIO/LPM configuration flag

        setup_all_GPIO_ports(false);
        setup_push_button();
        if (button_pressed)
        {
            button_pressed = 0;
            blink_LED(GPIO_PORT_P1, GPIO_PIN0);
        }

        while(1)
        {
            MAP_PCM_shutdownDevice(PCM_LPM45);
        }
    }

    setup_all_GPIO_ports(true);
    setup_push_button();

    // GPIO setup for red LED
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);   // Red LED
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

#ifdef USE_POLLING
    uint8_t prev_status = GPIO_INPUT_PIN_LOW;
#endif
    while(1)
    {
#ifdef USE_POLLING

        uint8_t btn_status = MAP_GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);
        if (btn_status == GPIO_INPUT_PIN_LOW && btn_status != prev_status) {
            blink_LED(GPIO_PORT_P1, GPIO_PIN0);
        }
        prev_status = btn_status;
#else
        MAP_PCM_shutdownDevice(PCM_LPM45);
#endif
    }
}

// Port1 interrupt handler
void PORT1_IRQHandler(void)
{
    uint16_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1)
        button_pressed = 1;
}
