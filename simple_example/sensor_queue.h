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
#define PROX1_DATA      0x1000000000000000
#define PROX2_DATA      0x2000000000000000
#define RGB_DATA        0x3000000000000000
#define TYPE_MASK       0xF000000000000000
#define VALUE_MASK      0x0FFFFFFFFFFFFFFF

/* Queue variables */
#define qLENGTH         32
#define qITEMSIZE       8

/* Struct used in RETURNING message from queue */
struct qData {
    long long int type;
    int value;
};

/* Routine declarations */
bool setupQs();
int sendToProx1Q(int value);
int sendToProx2Q(int value);
int sendToRGBQ(int r, int g, int b);
void receiveFromProx1Q(struct qData *oldData);
void receiveFromProx2Q(struct qData *oldData);
void receiveFromRGBQ(struct qData *oldData);

#endif /* SENSOR_QUEUE_H_ */

