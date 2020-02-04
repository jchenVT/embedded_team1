/*
 * sensor_queue.c
 *
 *  Created on: Feb 3, 2020
 *      Author: trieu
 */

/* RTOS header files */
#include <FreeRTOS.h>
#include <queue.h>

#define SUCCESS         0
#define FAILURE         1
#define TIME_DATA       0
#define SENSOR_DATA     1
#define QUEUE_LENGTH    32
#define QUEUE_ITEMSIZE  8

QueueHandle_t msgQ;
// xQueueCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize );

int createQ1() {

     // msgQ = xQueueCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize );

    return 1;
}

/*
 * BaseType_t xQueueSendToBackFromISR( QueueHandle_t xQueue,
         const void * pvItemToQueue,
         TickType_t xTicksToWait );

    refer to page 110
    value to use --> portMAX_DELAY
 */

int sendTimeMsgToQ1(unsigned int timeVal) {

    return 1;
}

int sendSensorMsgToQ1() {

    return 1;
}

/*
 * BaseType_t xQueueReceiveFromISR( QueueHandle_t xQueue,
         void * const pvBuffer,
         TickType_t xTicksToWait );

    refer to page 113
 */

unsigned long long int receiveFromQ1() {

    return 1;
}

void *queueThread(void *arg0) {
    /*
    createQ1();
    */

    while (1) {}

    return (NULL);
}
