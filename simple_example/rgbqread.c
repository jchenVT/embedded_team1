/*
 * rgbqread.c
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#include "rgbqread.h"


/*
 *  @function   rgbQReadThread
 *              Main thread that will pull from the sensor queue to
 *              package and sent to MQTT.
 *
 *  @params     arg0
 *  @return     void
 */
void *rgbQReadThread(void *arg0) {

    dbgOutputLoc(INIT_RGBQREAD);

    struct qData data = {0, 0}; // create default data to be changed

    while (1) {

        dbgOutputLoc(WAIT_RGBQREAD);

        /* Perform blocking read */
        receiveFromRGBQ(&data);

        dbgOutputLoc(RECV_RGBQREAD);

        /* Verify data is valid (and print to UART) */
        if (data.type == RGB_DATA) {

            /* Convert msg to HEX representation */
            int i;
            char sensorOut[32];
            int size = snprintf(sensorOut, 32, "RGB:0x%X", data.value);
            for (i=0; i<size; i++) {
                dbgUARTVal(sensorOut[i]);
            }
        }
        else {
            stop_all(FAILED_RGBQ_BADVAL);
        }
        dbgUARTVal(' ');

        /* TODO: Send to MQTT */
    }
}
