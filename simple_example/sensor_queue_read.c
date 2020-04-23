/*
 * sensor_queue_read.c
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#include "sensor_queue_read.h"


/*
 *  @function   sensorQReadThread
 *              Main thread that will pull from the sensor queue to
 *              package and sent to MQTT.
 *
 *  @params     arg0
 *  @return     void
 */
void *sensorQReadThread(void *arg0) {

    dbgOutputLoc(INIT_PQ1READ); // start proximity 1 read

    struct qData data = {0, 0, 0, 0, 0}; // create default data to be changed

    while (1) {

        dbgOutputLoc(WAIT_PQ1READ);

        /* Perform blocking read */
        receiveSensorQ(&data);

        dbgOutputLoc(RECV_PQ1READ);

        /* Verify data is valid */
        if (data.sensorID == PROX1_DATA) {
            if (data.sensorVal != 0 && data.sensorVal != 1) {
                stop_all(FAILED_PQ1_BADVAL);
            }
            if (data.r != 0 || data.g != 0 || data.b != 0) {
                stop_all(FAILED_PQ1_BADVAL);
            }
        }
        if (data.sensorID == PROX2_DATA) {
            if (data.sensorVal != 0 && data.sensorVal != 1) {
                stop_all(FAILED_PQ2_BADVAL);
            }
            if (data.r != 0 || data.g != 0 || data.b != 0) {
                stop_all(FAILED_PQ2_BADVAL);
            }
        }
        if (data.sensorID == RGB_DATA) {
            if (data.sensorVal != 0) {
                stop_all(FAILED_RGBQ_BADVAL);
            }
        }

        /* TODO: Send to MQTT */
        //packageArmSensorJSON(data.sensorID, data.sensorVal, data.r, data.g, data,b);

    }
}
