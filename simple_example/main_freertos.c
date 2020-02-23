#include <stdint.h>

#ifdef __ICCARM__
#include <DLib_Threads.h>
#endif

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <FreeRTOS.h>
#include <task.h>

/* Driver configuration */
#include <ti/drivers/Board.h>
#include "prox1read.h"
#include "prox2read.h"
//#include "rgbread.h"
#include "sensor_queue.h"
#include "debug.h"

/* Get thread locations */
extern void *readProximity1Thread(void *arg0);
//extern void *readProximity2Thread(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE   1024

/*
 *  ======== main ========
 */
int main(void)
{
    /* UART0 Proximity Sensor Read */
    pthread_t           thread1;
    pthread_attr_t      attrs1;
    int                 retcProx1;

    //pthread_t           thread2;
    //pthread_attr_t      attrs2;
    //int                 retcProx2;

    /* initialize the system locks */
    #ifdef __ICCARM__
        __iar_Initlocks();
    #endif

    /* Call driver setup functions */
    Board_init();
    //debug_setup();
    if (!setupQs()) {
        //stop_all(FAIL_Q1_INIT);
    }

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrs1);
    //pthread_attr_init(&attrs2);

    retcProx1 = pthread_create(&thread1, &attrs1, readProximity1Thread, NULL);
    //retcProx2 = pthread_create(&thread2, &attrs2, readProximity2Thread, NULL);

    if (retcProx1 != 0) {
        /* pthread_create() failed */
        while (1) {}
    }

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    return (0);
}

/**
 *  vApplicationMallocFailedHook()
 *      Application defined malloc failed hook
 */
void vApplicationMallocFailedHook() {

    /* Handle Memory Allocation Errors */
    while(1) {

    }
}


/**
 *  vApplicationStackOverflowHook()
 *      Application defined stack overflow hook
 */
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {

    /* Handle FreeRTOS Stack Overflow */
    while(1) {

    }
}
