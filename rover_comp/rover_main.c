/*
 * star.c
 *
 *  Created on: Feb 5, 2020
 *      Author: giffen
 */

#include <rover_main.h>

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

    // sensor queue
    struct receiveData data = {false, 0, 0};
    // sensor FSM
    //struct fsmData fsm = {Init, 0, 0, 0, 0};

    while(1) {
        /**********************************/
        dbgOutputLoc(STAR_WHILE_BEGIN); 
        /**********************************/

        // wait for message (blocking)
        // ADD DEBUG: BEFORE RECEIVING FROM QUEUE (NOT ISR)
        /**********************************/
        dbgOutputLoc(STAR_WAIT_MESSAGE); 
        /**********************************/
        receiveFromMQTTReceiveQ(&data);
        // ADD DEBUG: AFTER RECEIVING FROM QUEUE (NOT ISR)
        /**********************************/
        dbgOutputLoc(STAR_RECEIVE_MESSAGE);
        /**********************************/

    }
}
