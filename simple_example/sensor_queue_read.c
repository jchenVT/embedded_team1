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

    struct qData data = {0, 0, 0, 0, 0}; // create default data to be changed

    while (1) {

        /* Perform blocking read */
        receiveSensorQ(&data);

        /* Verify data is valid */
        if (data.sensorID == PROX1_DATA) {
            if (data.sensorVal != 0 && data.sensorVal != 1) {
                stop_all();
            }
            if (data.r != 0 || data.g != 0 || data.b != 0) {
                stop_all();
            }
        }
        if (data.sensorID == PROX2_DATA) {
            if (data.sensorVal != 0 && data.sensorVal != 1) {
                stop_all();
            }
            if (data.r != 0 || data.g != 0 || data.b != 0) {
                stop_all();
            }
        }
        if (data.sensorID == RGB_DATA) {
            if (data.sensorVal != 0) {
                stop_all();
            }
        }

        /* Send to MQTT */
        packageArmSensorJSON(data.sensorID, data.sensorVal, data.r, data.g, data.b);
    }
}
