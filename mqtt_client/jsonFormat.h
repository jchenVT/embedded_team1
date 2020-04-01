/*
 * jsonFormat.h
 *
 *  Created on: Mar 31, 2020
 *      Author: trieu
 */

#ifndef JSONFORMAT_H_
#define JSONFORMAT_H_


#include <stdio.h>
#include <stdlib.h>
#include "mqtt_queue.h"
#include <stdbool.h>
#include "jsmn.h"
#include <string.h>

#define MaxTokens       50
enum topicVal {ARM, ARM_SENSOR, ROVER, ROVER_SENSOR};

int jsonParser(const char *topic, char *JSON_STRING);

int packageArmJSON(int state);
int packageArmSensorJSON(int sensorID, int sensorValue);
int packageRoverJSON(int state);
int packageRoverSensorJSON(bool move_to_point, int point_x, int point_y, int angle_rotate);

#endif /* JSONFORMAT_H_ */
