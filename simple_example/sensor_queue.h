/*
 * sensor_queue.h
 *
 *  Created on: Feb 4, 2020
 *      Author: trieu
 */

#ifndef SENSOR_QUEUE_H_
#define SENSOR_QUEUE_H_

/* RTOS header files */
#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>

/* Masks for variable type */
#define TIME_DATA       0x1000000000000000
#define SENSOR_DATA     0x2000000000000000
/* Queue varialbes */
#define qLENGTH         32
#define qITEMSIZE       8

/* Declaration of the queue */
extern QueueHandle_t msgQ;

/* Struct used in RETURNING message from queue */
struct qData {
    bool success;
    int type;
    int value;
};

/* Routine declarations */
bool createQ1();
int sendTimeMsgToQ1(unsigned int timeVal);
int sendSensorMsgToQ1(int mmDist);
void receiveFromQ1(struct qData *oldData);

#endif /* SENSOR_QUEUE_H_ */

