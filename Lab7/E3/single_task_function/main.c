#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/drivers/Board.h>
#define __MSP432P4XX__
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define TASKSTACKSIZE   2048

//![Simple GPIO Config]
#define RGBLED_PORT  GPIO_PORT_P2
#define RLED_PIN  GPIO_PIN0
#define GLED_PIN  GPIO_PIN1

Void clockHandler1(UArg arg);
Void gpio_task(UArg arg0, UArg arg1);

Task_Struct task1Struct, task2Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE];

int main()
{
    /* Construct BIOS objects */
    Task_Params taskParams;

    /* Call driver init functions */
    Board_init();

    /* Construct task threads */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    taskParams.arg0 = RGBLED_PORT; // set port in first arg
    taskParams.arg1 = RLED_PIN; // set red pin in second arg
    Task_construct(&task1Struct, (Task_FuncPtr)gpio_task, &taskParams, NULL);

    taskParams.stack = &task2Stack;

    taskParams.arg1 = GLED_PIN; // before starting second task, set green pin num in second arg
    Task_construct(&task2Struct, (Task_FuncPtr)gpio_task, &taskParams, NULL);

    // Create clock that calls Task_yield() to preempt tasks
    Clock_Params clockParams;
    Clock_Handle myClk0;
    Clock_Params_init(&clockParams);
    clockParams.period = 10;         // 1 Clock tick is 1 ms by default
    clockParams.startFlag = TRUE;    // Start immediately
    myClk0 = Clock_create((Clock_FuncPtr)clockHandler1, 1, &clockParams, NULL);   // 2nd argument must be greater than 0
    if (myClk0 == NULL)
        System_abort("Clock0 create failed");

    BIOS_start();    /* Does not return */
    return(0);
}

Void gpio_task(UArg arg0, UArg arg1)
{
    uint_fast8_t port = (uint_fast8_t)arg0;
    uint_fast16_t pin_num = (uint_fast16_t)arg1;
    uint32_t sleep_delay = pin_num == RLED_PIN ? 2000: 500;
    printf("Port number (arg0): 0x%x, Pin number (arg1): 0x%x, sleep_delay: %d\n", port, pin_num, sleep_delay);
    MAP_GPIO_setAsOutputPin(port, pin_num);

    // Bring LED to known state
    MAP_GPIO_setOutputLowOnPin(port, pin_num);


    while(1) {
        MAP_GPIO_toggleOutputOnPin(RGBLED_PORT, pin_num);
        Task_sleep(sleep_delay); // 2 Hz if its red, 0.5 if its green
    }
}

Void clockHandler1(UArg arg)
{
    Task_yield();                   // To force a task switch if needed
}
