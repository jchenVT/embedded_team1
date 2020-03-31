#include <stdio.h>
#include <stdlib.h>
#include <mqtt_queue.h>
#include <stdbool.h>
#include <jsmn.h>
#include <string.h>
#include "jsonFormat.h"

#define JSON_LEN 120
jsmn_parser parser;

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

int jsonParser(enum topicVal topic, char * JSON_STRING) {

    static bool initParser = false;

    if (!initParser) {
        jsmn_init(&parser);
    }

    jsmntok_t tokens[MaxTokens];
    int num;

    num = jsmn_parse(&parser, JSON_STRING, strlen(JSON_STRING), tokens, MaxTokens);

    if (num == 0) {
        // ERROR
    }

    switch(topic) {
        case ARM:
            // Currently does not check data
            if (jsoneq(JSON_STRING, &tokens[0], "state") != 0 || num != 2) {
                // ERROR
            }

            char *state;
            strncpy(state, JSON_STRING + tokens[1].start, tokens[1].end - tokens[1].start);

            pushToArmQ(atoi(state));
            break;

        case ARM_SENSOR:
            if (jsoneq(JSON_STRING, &tokens[0], "move_to_point") != 0 ||
                    jsoneq(JSON_STRING, &tokens[2], "point_x") != 0 ||
                    jsoneq(JSON_STRING, &tokens[4], "point_y") != 0 ||
                    jsoneq(JSON_STRING, &tokens[6], "angle_rotate") != 0 ||
                    num != 8) {
                // ERROR
            }

            char *movePoint;
            strncpy(movePoint, JSON_STRING + tokens[1].start, tokens[1].end - tokens[1].start);

            char *ptr;
            double pointX = strtod(JSON_STRING + tokens[3].start, &ptr);

            char *ptr;
            double pointY = strtod(JSON_STRING + tokens[5].start, &ptr);

            char *ptr;
            double angleRotate = strtod(JSON_STRING + tokens[7].start, &ptr);

            pushToArmSensorQ(atoi(movePoint), pointX, pointY, angleRotate);
            break;

        case ROVER:
            if (jsoneq(JSON_STRING, &tokens[0], "state") != 0 || num != 2) {
                // ERROR
            }

            char *state;
            strncpy(state, JSON_STRING + tokens[1].start, tokens[1].end - tokens[1].start);

            pushToRoverQ(atoi(state));
            break;

        case ROVER_SENSOR:
            if (jsoneq(JSON_STRING, &tokens[0], "sensorID") != 0 || jsoneq(JSON_STRING, &tokens[2], "sensorValue") != 0 || num != 6) {
                // ERROR
            }

            char *sensorID;
            strncpy(sensorID, JSON_STRING + tokens[1].start, tokens[1].end - tokens[1].start);

            char *sensorValue;
            strncpy(sensorValue, JSON_STRING + tokens[3].start, tokens[3].end - tokens[3].start);

            pushToRoverSensorQ(atoi(sensorID), atoi(sensorValue));
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

