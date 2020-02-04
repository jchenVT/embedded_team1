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

#define TIME_DATA       0x10000000
#define SENSOR_DATA     0x20000000
#define qLENGTH         32
#define qITEMSIZE       8

extern QueueHandle_t msgQ;

struct qData {
    bool success;
    int type;
    int value;
};

bool createQ1();
int sendTimeMsgToQ1(unsigned int timeVal);
int sendSensorMsgToQ1(int mmDist);
void receiveFromQ1(struct qData *oldData);

#endif /* SENSOR_QUEUE_H_ */

