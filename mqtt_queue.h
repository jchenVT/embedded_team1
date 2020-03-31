/*
 * mqtt_queue.h
 *
 *  Created on: Feb 4, 2020
 *      Author: trieu
 */

#ifndef MQTT_QUEUE_H_
#define MQTT_QUEUE_H_

/* RTOS header files */
#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>

/* Queue variables */
#define SUB_COUNT       1
#define PUB_COUNT       1

#define qLENGTH         32
#define qITEMSIZE       8

/* Struct used in RETURNING message from queue */
struct qData {
    int topic;
    long long int msg;
};

/* Routine declarations */
bool createQs();
int sendToPubQ(int topic, long long int msg);
int sendToSubQ(int topic, long long int msg);
void receiveFromPubQ(struct qData *oldData);
void receiveFromSubQ(struct qData *oldData);

#endif /* MQTT_QUEUE_H_ */

