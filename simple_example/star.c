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
    /**********************************/
    dbgOutputLoc(STAR_MAIN_START); 
    /**********************************/

    // create queue
    bool q = createQ1();

    // sensor queue
    struct qData data = {true, 0, 0};
    int sensorVal = 0;
    int timeInc = 0;

    // sensor FSM
    struct fsmData fsm = {Init, 0, 0, 0, 0};

    // ADD DEBUG: WHILE
    while(1 && q) {
        /**********************************/
        dbgOutputLoc(STAR_WHILE_BEGIN); 
        /**********************************/

        // wait for message (blocking)
        // ADD DEBUG: BEFORE RECEIVING FROM QUEUE (NOT ISR)
        /**********************************/
        dbgOutputLoc(STAR_WAIT_MESSAGE); 
        /**********************************/
        receiveFromQ1(&data);
        // ADD DEBUG: AFTER RECEIVING FROM QUEUE (NOT ISR)
        /**********************************/
        dbgOutputLoc(STAR_RECEIVE_MESSAGE);
        /**********************************/

        // check for success
        if (!data.success) {
            stop_all();
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
            stop_all();
        }

        // change fsm (in sensor_states)
        /**********************************/
        dbgOutputLoc(STAR_UPDATE_TASK); 
        /**********************************/
        updateFSM(&fsm, timeInc, sensorVal);
    }

    stop_all();

    return 0;
}
