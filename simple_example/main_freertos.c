#include <stdint.h>

#ifdef __ICCARM__
#include <DLib_Threads.h>
#endif

#include <stdlib.h>

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <FreeRTOS.h>
#include <task.h>

/* Driver configuration */
#include <ti/drivers/Board.h>
#include <ti/drivers/SPI.h>

/* Thread files */
#include "sensor_read.h"

/* Queue files */
#include "sensor_queue_read.h"
#include "sensor_queue.h"
#include "mqtt_queue.h"

/* Debug files */
#include "uart_term.h"

/* Get thread locations */
extern void *mainThread(void *arg0);
extern void *readSensorsThread(void *arg0);
extern void *sensorQReadThread(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE   8192

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
    SPI_init();
    InitTerm();

    if (!setupQ()) {
        stop_all();
    }

    if (!createMQTTQs()) {
        stop_all();
    }

    /* Sensor Read */
    pthread_t           threadSensor;
    pthread_attr_t      attrsSensor;
    int                 retcSensor;

    /* Sensor Queue Read */
    pthread_t           threadSensorQ;
    pthread_attr_t      attrsSensorQ;
    int                 retcSensorQ;

    /* Sensor Queue Read */
    pthread_t           threadMQTT;
    pthread_attr_t      attrsMQTT;
    int                 retcMQTT;

    pthread_attr_init(&attrsSensor);
    pthread_attr_init(&attrsSensorQ);
    pthread_attr_init(&attrsMQTT);

    retcMQTT = pthread_create(&threadMQTT, &attrsMQTT, mainThread, NULL);
    retcSensor = pthread_create(&threadSensor, &attrsSensor, readSensorsThread, NULL);
    retcSensorQ = pthread_create(&threadSensorQ, &attrsSensorQ, sensorQReadThread, NULL);

    if (retcSensor != 0 && retcSensorQ != 0 && retcMQTT != 0) {
        stop_all();
        while (1) {
        }
    }

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    return (0);
}

/*
 *  @function    vApplicationMallocFailedHook()
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
