#include <stdio.h>
#include <mqtt_queue.h>
#include <stdbool.h>
#include <jsmn.h>
#include <string.h>
#include "jsonFormat.h"

#define JSON_LEN 120
jsmn_parser parser;

int jsonParser(char * jsonMsg) {

    static bool initParser = false;

    if (!initParser) {
        jsmn_init(&parser);
    }

    jsmntok_t tokens[MaxTokens];
    int num;

    num = jsmn_parse(&parser, jsonMsg, strlen(jsonMsg), tokens, MaxTokens);

    if (num == 0 || jsoneq(JSON_STRING, &tokens[0], "Topic") != 0) {
        // ERROR
    }

    int topic = atoi(tokens[1].end - tokens[1].start);

    switch(topic) {
        case ARM:
            // Currently does not check data
            if (jsoneq(JSON_STRING, &tokens[2], "State") != 0 || num != 4) {
                // ERROR
            }

            uint8_t state = atoi(tokens[3].end - tokens[3].start);

            pushToArmQ(state);
            break;

        case ARM_SENSOR:
            if (jsoneq(JSON_STRING, &tokens[2], "Move_to_point") != 0 ||
                    jsoneq(JSON_STRING, &tokens[4], "Point_x") != 0 ||
                    jsoneq(JSON_STRING, &tokens[6], "Point_y") != 0 ||
                    jsoneq(JSON_STRING, &tokens[8], "Angle_rotate") != 0 ||
                    num != 10) {
                // ERROR
            }

            bool movePoint = atoi(tokens[3].end - tokens[3].start);
            double pointX = atoi(tokens[5].end - tokens[5].start);
            double pointY = atoi(tokens[7].end - tokens[7].start);
            double angleRotate = atoi(tokens[9].end - tokens[9].start);

            pushToArmSensorQ(movePoint, pointX, pointY, angleRotate);
            break;

        case ROVER:
            if (jsoneq(JSON_STRING, &tokens[2], "State") != 0 || num != 4) {
                // ERROR
            }

            uint8_t state = atoi(tokens[3].end - tokens[3].start);

            pushToRoverQ(state);
            break;

        case ROVER_SENSOR:
            if (jsoneq(JSON_STRING, &tokens[2], "SensorID") != 0 || jsoneq(JSON_STRING, &tokens[4], "SensorValue") != 0 || num != 6) {
                // ERROR
            }

            int sensorID = atoi(tokens[3].end - tokens[3].start);
            int sensorValue = atoi(tokens[5].end - tokens[5].start);

            pushToRoverSensorQ(sensorID, sensorValue);
            break;
    }

}

int packageArmJSON(int state) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{ \"state\": %d }", state);
//    return sendToPubQ(0, &json);
    return 1;
}

int packageArmSensorJSON(int sensorID, int sensorValue) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{ \"sensorID\": %d, \"sensorValue\": %d }", sensorID, sensorValue);
//    return sendToPubQ(1, &json);
    return 1;
}

int packageRoverJSON(int state) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{ \"state\": %d }", state);
//    return sendToPubQ(2, &json);
    return 1;
}

int packageRoverSensorJSON(bool move_to_point, int point_x, int point_y, int angle_rotate) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{ \"move_to_point\": %s, \"point_x\": %d, \"point_y\": %d, \"angle_rotate\": %d }", move_to_point ? "true"; "false", point_x, point_y, angle_rotate);
//    return sendToPubQ(3, &json);
    return 1;
}

