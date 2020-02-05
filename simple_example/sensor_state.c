/*
 * sensor_state.c
 *
 *  Created on: Feb 4, 2020
 *      Author: trieu, giffen
 */

#include "sensor_state.h"

/*
 *  @function   updateFSM
 *              Changes the state of the FSM based on the current
 *              sensor value and timer inc.
 *
 *  @params     timeInc, sensorVal
 */
int updateFSM(struct fsmData *fsm, int timeInc, int sensorVal) {

    switch (fsm->curState) {
        case Init:
            fsm->curTime = 0;
            fsm->sensorTotal = 0;
            fsm->sensorCount = 0;
            fsm->sensorAvg = 0;
            fsm->curState = WaitingForTime1;
            break;

        case WaitingForTime1:
            if (timeInc == 0) {
                fsm->sensorTotal += sensorVal;
                fsm->sensorCount++;
            }
            if (timeInc > 0) {
                fsm->curTime += timeInc;
                fsm->sensorAvg = fsm->sensorTotal/fsm->sensorCount;
                // print uart sensor =
                fsm->sensorTotal = 0;
                fsm->sensorCount = 0;
                fsm->curState = WaitingForTime2;
            }
            break;

        case WaitingForTime2:
            if (timeInc == 0) {
                fsm->sensorTotal += sensorVal;
                fsm->sensorCount++;
            }
            if (timeInc > 0) {
                fsm->curTime += timeInc;
                fsm->sensorAvg = fsm->sensorTotal/fsm->sensorCount;
                // print uart sensor=
                fsm->sensorTotal = 0;
                fsm->sensorCount = 0;
                fsm->curState = WaitingForTime3;
            }
            break;

        case WaitingForTime3:
            if (timeInc == 0) {
                fsm->sensorTotal += sensorVal;
                fsm->sensorCount++;
            }
            if (timeInc > 0) {
                fsm->curTime += timeInc;
                fsm->sensorAvg = fsm->sensorTotal/fsm->sensorCount;
                // print uart sensor =
                // print uart time =
                fsm->sensorTotal = 0;
                fsm->sensorCount = 0;
                fsm->curState = WaitingForTime1;
            }
            break;
    }

    return 1;
}
