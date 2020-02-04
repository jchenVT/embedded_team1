/*
 * sensor_queue.h
 *
 *  Created on: Feb 4, 2020
 *      Author: trieu
 */

#ifndef SENSOR_QUEUE_H_
#define SENSOR_QUEUE_H_

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

