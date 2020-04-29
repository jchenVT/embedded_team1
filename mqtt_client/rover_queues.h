/*
 * rover_queues.h
 *
 *  Created on: Feb 19, 2020
 *      Author: trieu
 */

#ifndef ROVER_QUEUES_H_
#define ROVER_QUEUES_H_

/* RTOS header files */
#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>

#include <rover_debug.h>

/* Motor Controller Data */
#define FORWARD_COMMAND 0b00000000
#define REVERSE_COMMAND 0b00000001
#define FR_WHEEL_ADDR   0b10000000
#define FL_WHEEL_ADDR   0b10000001
#define BM_WHEEL_ADDR   0b10000010

/* Struct used in RETURNING message from queue */
struct motorData {
    char address;
    char command;
    char speed;
};

struct receiveData {
    bool sensorType;
    bool point_move;
    int angle_rotate;
    double data;
    double data2;
};

/* Routine declarations */
bool createMotorQ();
bool createEncoderQ();
bool createMQTTReceiveQ();

int sendMsgToMotorsQ(char address, char command, char speed);
int sendMsgToEncoderQ();
int sendMsgToReceiveQ(bool sensorType, bool move, int angle_rotate, double data, double data2);

bool receiveFromMotorsQ(struct motorData *oldData);
bool receiveFromEncoderQ(bool *oldData);
bool receiveFromMQTTReceiveQ(struct receiveData *oldData);

#endif /* ROVER_QUEUES_H_ */

