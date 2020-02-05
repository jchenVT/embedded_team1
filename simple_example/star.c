/*
 * star.c
 *
 *  Created on: Feb 5, 2020
 *      Author: giffen
 */

#include "star.h"

/*
 *  @function   mainThread
 *              Main thread that will pull from the sensor queue to
 *              change the FSM states.
 *
 *  @params     arg0
 *  @return     void
 */
void *mainThread(void *arg0)
{
    // create queue
    bool q = createQ1();

    // sensor queue
    struct qData data = {true, 0, 0};
    int sensorVal = 0;
    int timeInc = 0;

    // sensor FSM
    struct fsmData fsm = {Init, 0, 0, 0, 0};

    while(1 && q) {
        // wait for message (blocking)
        receiveFromQ1(&data);

        // check for success
        if (data.success) {
            // error handle
        }

        // store values locally
        if (data.type == TIME_DATA) {
            timeInc = data.value;
        }
        else if (data.type == SENSOR_DATA) {
            timeInc = 0;
            sensorVal = data.value;
        }
        else {
            // error handle
        }

        // change fsm (in sensor_states)
        updateFSM(fsm, timeInc, sensorVal);
    }

    return 0;
}
