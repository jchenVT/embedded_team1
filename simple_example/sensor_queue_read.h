
/*
 * sensor_queue_read.h
 *
 *  Created on: Apr 20, 2020
 *      Author: giffen
 */

#ifndef SENSOR_QUEUE_READ_H_
#define SENSOR_QUEUE_READ_H_

#include "sensor_queue.h"
#include "json_format.h"
#include "debug.h"

void *sensorQReadThread(void *arg0);

#endif /* SENSOR_QUEUE_READ_H_ */
