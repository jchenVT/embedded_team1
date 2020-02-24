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
#include "proxread.h"
//#include "rgbread.h"
#include "sensor_queue.h"
#include "debug.h"

/* Get thread locations */
extern void *uartThread(void *arg0);
extern void *readProximityThread(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE   1024

/*
 *  ======== main ========
 */
int main(void) {

    /* initialize the system locks */
    #ifdef __ICCARM__
        __iar_Initlocks();
    #endif

    /* Call driver setup functions */
    Board_init();
    GPIO_init();
    debug_setup();

    if (!setupQs()) {
        stop_all(FAILED_INIT_QUEUES);
    }

    dbgOutputLoc(INIT_THREADS);

    /* Debug UART */
    pthread_t           threadDebug;
    pthread_attr_t      attrsDebug;
    int                 retcDebug;

    /* Proximity Sensor Read */
    pthread_t           threadProx;
    pthread_attr_t      attrsProx;
    int                 retcProx;

    /* Other thread... */
    //pthread_t           thread2;
    //pthread_attr_t      attrs2;
    //int                 retcProx2;

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrsDebug);
    pthread_attr_init(&attrsProx);

    retcDebug = pthread_create(&threadDebug, &attrsDebug, uartThread, NULL);
    retcProx = pthread_create(&threadProx, &attrsProx, readProximityThread, NULL);

    if (retcDebug != 0 && retcProx != 0) {

        /* pthread_create() failed */
        stop_all(FAILED_INIT_THREADS);
        while (1) {

        }
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
