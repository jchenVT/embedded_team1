/*
 * sensor_state.c
 *
 *  Created on: Feb 4, 2020
 *      Author: trieu, giffen
 */

#include "debug.h"
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
                const char sensorOut[40];
                snprintf(sensorOut, 40, "Sensor=%d,%d\n", fsm->sensorAvg, fsm->sensorCount);
                UART_write(uart, sensorOut, sizeof(sensorOut));
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
                const char sensorOut[40];
                snprintf(sensorOut, 40, "Sensor=%d,%d\n", fsm->sensorAvg, fsm->sensorCount);
                UART_write(uart, sensorOut, sizeof(sensorOut));
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
                const char sensorOut[40];
                snprintf(sensorOut, 40, "CurTime=%d\n", fsm->curTime/1000);
                UART_write(uart, sensorOut, sizeof(sensorOut));
                fsm->sensorTotal = 0;
                fsm->sensorCount = 0;
                fsm->curState = WaitingForTime1;
            }
            break;
    }

    return 1;
}
