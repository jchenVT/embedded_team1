/*
 * sensor_queue.h
 *
 *  Created on: Feb 4, 2020
 *      Author: trieu, giffen
 */

#ifndef SENSOR_QUEUE_H_
#define SENSOR_QUEUE_H_

/* RTOS header files */
#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>

/* IDs for sensors */
#define PROX1_DATA      1
#define PROX2_DATA      2
#define RGB_DATA        3

/* Queue variables */
#define qLENGTH         32

/* Struct used in RETURNING message from queue */
struct qData {
    int sensorID;
    int sensorVal;
    int r;
    int g;
    int b;
};

/* Routine declarations */
bool setupQ();
int sendProxToSensorQ(int proxNum, int val);
int sendRGBToSensorQ(int r, int g, int b);
void receiveSensorQ(struct qData *oldData);

#endif /* SENSOR_QUEUE_H_ */

