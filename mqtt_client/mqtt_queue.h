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
#include <stdio.h>
#include <string.h>

/* Queue variables */
#define SUB_COUNT       1
#define PUB_COUNT       1

#define qLENGTH         12
#define qITEMSIZE       8

/* Struct used in RETURNING message from queue */
struct qData {
    int topic;
    long long int msg;
};

struct qStringData {
    int topic;
    char str[120];
};

struct qArmMsg {
    int state;
};

struct qArmSensorMsg {
    int sensorID;
    int sensorValue;
};

struct qRoverMsg {
    int state;
};

struct qRoverSensorMsg {
    bool move_to_point;
    int point_x;
    int point_y;
    int angle_rotate;
};

/* Routine declarations */
bool createQs();
int sendToPubQ(int topic, char msg []);
int receiveFromPubQ(struct qStringData *oldData);


int sendToSubArmQ(struct qArmMsg msg);
int sendToSubArmSensorQ(struct qArmSensorMsg msg);
int sendToSubRoverQ(struct qRoverMsg msg);
int sendToSubRoverSensorQ(struct qRoverSensorMsg msg);

int receiveFromSubQ(struct qData *oldData);

int receiveFromSubArmQ(struct qArmMsg *oldData);
int receiveFromSubArmSensorQ(struct qArmSensorMsg *oldData);
int receiveFromSubRoverQ(struct qRoverMsg *oldData);
int receiveFromSubRoverSensorQ(struct qRoverSensorMsg *oldData);

#endif /* MQTT_QUEUE_H_ */

