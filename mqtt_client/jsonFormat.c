#include "jsonFormat.h"
#include "mqtt_queue.h"
#include "jsmn.h"

static jsmn_parser parser;

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

int jsonParser(const char *topic, char *JSON_STRING) {

    static bool initParser = false;

    if (!initParser) {
        jsmn_init(&parser);
    }

    jsmntok_t tokens[MaxTokens];
    int num;

    num = jsmn_parse(&parser, JSON_STRING, strlen(JSON_STRING), tokens, MaxTokens);

    if (num == 0) {
        // ERROR
        return 0;
    }

    switch(atoi(topic)) {
    {
        case ARM:
            // Currently does not check data
            if (jsoneq(JSON_STRING, &tokens[0], "state") != 0 || num != 2) {
                // ERROR
                return 0;
            }

            char state;
            strncpy(&state, JSON_STRING + tokens[1].start, tokens[1].end - tokens[1].start);

            struct qArmMsg armMsg = {state-'0'};

            sendToSubArmQ(armMsg);
            break;
    }
    {
        case ROVER_SENSOR:
            if (jsoneq(JSON_STRING, &tokens[0], "move_to_point") != 0 ||
                    jsoneq(JSON_STRING, &tokens[2], "point_x") != 0 ||
                    jsoneq(JSON_STRING, &tokens[4], "point_y") != 0 ||
                    jsoneq(JSON_STRING, &tokens[6], "angle_rotate") != 0 ||
                    num != 8) {
                // ERROR
                return 0;
            }

            struct qRoverSensorMsg roverSensorMsg;

            char *ptr;
            char movePoint;
            strncpy(&movePoint, JSON_STRING + tokens[1].start, tokens[1].end - tokens[1].start);

            roverSensorMsg.move_to_point = movePoint-'0';
            roverSensorMsg.point_x = strtod(JSON_STRING + tokens[3].start, &ptr);
            roverSensorMsg.point_y = strtod(JSON_STRING + tokens[5].start, &ptr);
            roverSensorMsg.angle_rotate = strtod(JSON_STRING + tokens[7].start, &ptr);

            sendToSubRoverSensorQ(roverSensorMsg);
            break;
    }
    {
        case ROVER:
            if (jsoneq(JSON_STRING, &tokens[0], "state") != 0 || num != 2) {
                // ERROR
                return 0;
            }

            char roverState;
            strncpy(&roverState, JSON_STRING + tokens[1].start, tokens[1].end - tokens[1].start);

            struct qRoverMsg roverMsg = {roverState-'0'};

            sendToSubRoverQ(roverMsg);
            break;
    }
    {
        case ARM_SENSOR:
            if (jsoneq(JSON_STRING, &tokens[0], "sensorID") != 0 || jsoneq(JSON_STRING, &tokens[2], "sensorValue") != 0 || num != 6) {
                // ERROR
                return 0;
            }

            char sensorID[4];
            strncpy(sensorID, JSON_STRING + tokens[1].start, tokens[1].end - tokens[1].start);

            char sensorValue[4];
            strncpy(sensorValue, JSON_STRING + tokens[3].start, tokens[3].end - tokens[1].start);

            struct qArmSensorMsg armSensorMsg = {atoi(sensorID), atoi(sensorValue)};

            sendToSubArmSensorQ(armSensorMsg);
            break;
    }
    }
    return 1;
}

int packageArmJSON(int state) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{ \"state\": %d }", state);
    return sendToPubQ(ARM, json);
}

int packageArmSensorJSON(int sensorID, int sensorValue) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{ \"sensorID\": %d, \"sensorValue\": %d }", sensorID, sensorValue);
    return sendToPubQ(ARM_SENSOR, json);
}

int packageRoverJSON(int state) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{ \"state\": %d }", state);
    return sendToPubQ(ROVER, json);
}

int packageRoverSensorJSON(bool move_to_point, int point_x, int point_y, int angle_rotate) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{ \"move_to_point\": %s, \"point_x\": %d, \"point_y\": %d, \"angle_rotate\": %d }", move_to_point ? "true": "false", point_x, point_y, angle_rotate);
    return sendToPubQ(ROVER_SENSOR, json);
}

