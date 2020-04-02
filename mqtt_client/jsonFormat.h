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
#include <stdbool.h>
#include <string.h>

#define JSON_LEN        120
#define MaxTokens       50
enum topicVal {ARM, ARM_SENSOR, ROVER, ROVER_SENSOR};

int jsonParser(const char *topic, char *JSON_STRING);

int packageArmJSON(int state);
int packageArmSensorJSON(int sensorID, int sensorValue);
int packageRoverJSON(int state);
int packageRoverSensorJSON(bool move_to_point, int point_x, int point_y, int angle_rotate);
int packageDebugJSON(int attemptPubCount, int recvSubCount, bool status, char *subTopic, char *pubTopic);

#endif /* JSONFORMAT_H_ */
