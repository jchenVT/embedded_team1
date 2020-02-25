/*
 * proxq1read.c
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#include "proxq1read.h"


/*
 *  @function   proxQ1ReadThread
 *              Main thread that will pull from the sensor queue to
 *              package and sent to MQTT.
 *
 *  @params     arg0
 *  @return     void
 */
void *proxQ1ReadThread(void *arg0) {

    dbgOutputLoc(INIT_PQ1READ); // start proximity 1 read

    struct qData data = {0, 0}; // create default data to be changed

    while (1) {

        dbgOutputLoc(WAIT_PQ1READ);

        /* Perform blocking read */
        receiveFromProx1Q(&data);

        dbgOutputLoc(RECV_PQ1READ);

        /* Verify data is valid (and print to UART) */
        dbgUARTVal('P');
        dbgUARTVal('Q');
        dbgUARTVal('1');
        dbgUARTVal(':');
        if (data.type == PROX1_DATA) {
            if (data.value == 1) {
                dbgUARTVal('1');
            }
            else if (data.value == 0) {
                dbgUARTVal('0');
            }
            else {
                stop_all(FAILED_PQ1_BADVAL);
            }
        }
        else {
            stop_all(FAILED_PQ1_BADVAL);
        }
        dbgUARTVal(' ');

        /* TODO: Send to MQTT */
    }
}
