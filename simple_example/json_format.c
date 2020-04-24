#include "json_format.h"
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
        return -1;
    }

    int retVal = 0;

    if (strcmp(topic, "arm") == 0) {
            // Currently does not check data
            if (jsoneq(JSON_STRING, &tokens[1], "state") != 0 || num != 3) {
                return -2;
            }

            char state;
            strncpy(&state, JSON_STRING + tokens[2].start, tokens[2].end - tokens[2].start);

            struct qArmMsg armMsg = {state-'0'};

            retVal = sendToSubArmQ(armMsg);
    }
    else if (strcmp(topic, "rover_sensor") == 0) {
            if (jsoneq(JSON_STRING, &tokens[1], "move_to_point") != 0 ||
                    jsoneq(JSON_STRING, &tokens[3], "point_x") != 0 ||
                    jsoneq(JSON_STRING, &tokens[5], "point_y") != 0 ||
                    jsoneq(JSON_STRING, &tokens[7], "angle_rotate") != 0 ||
                    num != 9) {
                // ERROR
                return -3;
            }

            struct qRoverSensorMsg roverSensorMsg;

            char *ptr;
            char movePoint;
            strncpy(&movePoint, JSON_STRING + tokens[2].start, tokens[2].end - tokens[2].start);

            roverSensorMsg.move_to_point = movePoint-'0';
            roverSensorMsg.point_x = strtod(JSON_STRING + tokens[4].start, &ptr);
            roverSensorMsg.point_y = strtod(JSON_STRING + tokens[6].start, &ptr);
            roverSensorMsg.angle_rotate = strtod(JSON_STRING + tokens[8].start, &ptr);

            retVal = sendToSubRoverSensorQ(roverSensorMsg);
    }
    else if (strcmp(topic, "rover") == 0) {
            if (jsoneq(JSON_STRING, &tokens[1], "state") != 0 || num != 3) {
                // ERROR
                return -4;
            }

            char roverState;
            strncpy(&roverState, JSON_STRING + tokens[2].start, tokens[2].end - tokens[2].start);

            struct qRoverMsg roverMsg = {roverState-'0'};

            retVal = sendToSubRoverQ(roverMsg);
    }
    else if (strcmp(topic, "arm_sensor") == 0) {
            if (jsoneq(JSON_STRING, &tokens[1], "source") != 0 || jsoneq(JSON_STRING, &tokens[3], "message") != 0 || jsoneq(JSON_STRING, &tokens[5], "error") != 0 || num != 5) {
                // ERROR
                return -5;
            }

            char sourceString[1];
            strncpy(sourceString, JSON_STRING + tokens[2].start, tokens[2].end - tokens[2].start);

            char messageString[10];
            strncpy(messageString, JSON_STRING + tokens[4].start, tokens[4].end - tokens[4].start);

            char errorString[10];
            strncpy(errorString, JSON_STRING + tokens[4].start, tokens[4].end - tokens[4].start);

            struct qCommandMsg commandMsg = {atoi(sourceString), messageString, errorString};

            retVal = sendToSubCommandQ(commandMsg);
    }
    else if (strcmp(topic, "command") == 0) {
            if (jsoneq(JSON_STRING, &tokens[1], "sensorID") != 0 || jsoneq(JSON_STRING, &tokens[3], "sensorValue") != 0 || num != 7) {
                // ERROR
                return -5;
            }

            char sensorID[4];
            strncpy(sensorID, JSON_STRING + tokens[2].start, tokens[2].end - tokens[2].start);

            char sensorValue[4];
            strncpy(sensorValue, JSON_STRING + tokens[4].start, tokens[4].end - tokens[4].start);

            struct qArmSensorMsg armSensorMsg = {atoi(sensorID), atoi(sensorValue)};

            retVal = sendToSubArmSensorQ(armSensorMsg);
    }
    else {
        // ERROR TOPIC
        return -6;
    }

    return retVal;
}

int packageArmJSON(int state) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{\"state\": %d}", state);
    return sendToPubQ("arm", json);
}

int packageArmSensorJSON(int sensorID, int sensorValue, int r, int g, int b) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{\"sensorID\": %d, \"sensorValue\": %d, \"r\": %d, \"g\": %d, \"b\": %d}", sensorID, sensorValue, r, g, b);
    return sendToPubQ("arm_sensor", json);
}

int packageRoverJSON(int state) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{\"state\": %d}", state);
    return sendToPubQ("rover", json);
}

int packageRoverSensorJSON(bool move_to_point, int point_x, int point_y, int angle_rotate) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{\"move_to_point\": %s, \"point_x\": %d, \"point_y\": %d, \"angle_rotate\": %d}", move_to_point ? "true": "false", point_x, point_y, angle_rotate);
    return sendToPubQ("rover_sensor", json);
}

int packageDebugJSON(int attemptPubCount, int recvSubCount, bool status, char * subTopic, char * pubTopic) {
    char json[JSON_LEN];
    snprintf(json, JSON_LEN, "{\"attemptPubCount\": %d, \"recvSubCount\": %d, \"status\": \"%s\", \"subTopic\": \"%s\", \"pubTopic\": \"%s\"}", attemptPubCount, recvSubCount, status ? "true": "false", subTopic, pubTopic);
    return sendToPubQ("debug", json);
}
