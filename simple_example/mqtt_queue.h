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

#define mqttLENGTH      8
#define mqttITEMSIZE    8
#define MAXJSONSIZE     120

/* Struct used in RETURNING message from queue */
struct mqttData {
    char * topic;
    long long int msg;
};

struct qStringData {
    char * topic;
    char str[120];
};

struct qArmMsg {
    int state;
};

struct qArmSensorMsg {
    int sensorID;
    int sensorValue;
    int r;
    int g;
    int b;
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

struct qCommandMsg {
    int source;
    char msg[120];
    char err[120];
};

/* Routine declarations */
bool createMQTTQs();
int sendToPubQ(char * topic, char msg []);
int receiveFromPubQ(struct qStringData *oldData);

int sendToSubArmQ(struct qArmMsg msg);
int sendToSubArmSensorQ(struct qArmSensorMsg msg);
int sendToSubRoverQ(struct qRoverMsg msg);
int sendToSubRoverSensorQ(struct qRoverSensorMsg msg);
int sendToSubCommandQ(struct qCommandMsg msg);

int receiveFromSubArmQ(struct qArmMsg *oldData);
int receiveFromSubArmSensorQ(struct qArmSensorMsg *oldData);
int receiveFromSubRoverQ(struct qRoverMsg *oldData);
int receiveFromSubRoverSensorQ(struct qRoverSensorMsg *oldData);
int receiveFromSubCommandQ(struct qCommandMsg *oldData);

#endif /* MQTT_QUEUE_H_ */
