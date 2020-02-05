/*
 * sensor_state.h
 *
 *  Created on: Feb 4, 2020
 *      Author: trieu, giffen
 */

#ifndef SENSOR_STATE_H_
#define SENSOR_STATE_H_

enum states {Init, WaitingForTime1, WaitingForTime2, WaitingForTime3};

/* Struct used in store FSM data */
struct fsmData {
    enum states curState;
    int curTime;
    int sensorTotal;
    int sensorCount;
    int sensorAvg;
};

int updateFSM(struct fsmData fsm, int timeInc, int sensorVal);

#endif /* SENSOR_STATE_H_ */
