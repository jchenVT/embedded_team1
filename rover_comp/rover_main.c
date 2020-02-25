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

    struct receiveData curData = {false, 0, 0};
    // sensor FSM
    //struct fsmData fsm = {Init, 0, 0, 0, 0};

    while(1) {
        /**********************************/
        dbgOutputLoc(STAR_WHILE_BEGIN); 
        /**********************************/

        // wait for message (blocking)
        /**********************************/
        dbgOutputLoc(STAR_WAIT_MESSAGE); 
        /**********************************/
        receiveFromMQTTReceiveQ(&curData);

        if (curData.sensorType == false) {
            int i;
            dbgOutputLoc(sizeof(curData.data));
            for (i=0;i<sizeof(curData.data);i++) {
                dbgOutputLoc((curData.data >> 8*i));
            }

            if (curData.data2 == 128) {
                dbgOutputLoc(0x80);
            }
            else if (curData.data2 == 129) {
                dbgOutputLoc(0x81);
            }
            else if (curData.data2 == 130) {
                dbgOutputLoc(0x82);
            }
            else {
                dbgOutputLoc(0xFF);
            }
        }

        /**********************************/
        dbgOutputLoc(STAR_RECEIVE_MESSAGE);
        /**********************************/

    }
}
