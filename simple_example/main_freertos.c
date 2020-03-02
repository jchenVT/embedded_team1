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
#include "rgbread.h"
#include "proxq1read.h"
#include "sensor_queue.h"

/* Debug files */
#include "debug.h"

/* Get thread locations */
extern void *uartThread(void *arg0);
extern void *readProximityThread(void *arg0);
extern void *readRGBThread(void *arg0);
extern void *proxQ1ReadThread(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE   1024

/*
 *  @function   main
 *              main thread when starting the TI board.
 */
int main(void) {

    /* Initialize the system locks */
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

    /* Debug UART Output */
    pthread_t           threadDebug;
    pthread_attr_t      attrsDebug;
    int                 retcDebug;

    /* Proximity Sensor Read */
    //pthread_t           threadProx;
    //pthread_attr_t      attrsProx;
    //int                 retcProx;

    /* RGB Sensor Read */
    pthread_t           threadRGB;
    pthread_attr_t      attrsRGB;
    int                 retcRGB;


    /* P1 Queue Read */
    //pthread_t           threadP1Q;
    //pthread_attr_t      attrsP1Q;
    //int                 retcP1Q;

    pthread_attr_init(&attrsDebug);
    pthread_attr_init(&attrsRGB);
    pthread_attr_setstacksize(&attrsRGB, 8000);
    //pthread_attr_init(&attrsProx);
    //pthread_attr_init(&attrsP1Q);

    retcDebug = pthread_create(&threadDebug, &attrsDebug, uartThread, NULL);
    retcRGB = pthread_create(&threadRGB, &attrsRGB, readRGBThread, NULL);
    //retcProx = pthread_create(&threadProx, &attrsProx, readProximityThread, NULL);
    //retcP1Q = pthread_create(&threadP1Q, &attrsP1Q, proxQ1ReadThread, NULL);

    if (retcDebug != 0 && retcRGB != 0) { //retcProx != 0 && && retcP1Q != 0) {
        stop_all(FAILED_INIT_THREADS);
        while (1) {

        }
    }

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    return (0);
}

/*
 *  @funtion    vApplicationMallocFailedHook()
 *              Application defined malloc failed hook
 */
void vApplicationMallocFailedHook() {

    /* Handle Memory Allocation Errors */
    while(1) {

    }
}

/*
 *  @function   vApplicationStackOverflowHook()
 *              Application defined stack overflow hook
 */
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {

    /* Handle FreeRTOS Stack Overflow */
    while(1) {

    }
}
