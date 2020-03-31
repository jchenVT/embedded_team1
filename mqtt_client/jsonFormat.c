#include <stdio.h>
#include <mqtt_queue.h>
#include <stdbool.h>
#include <jsmn.h>
#include <string.h>

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
            pushToArmQ();
            break;

        case ARM_SENSOR:
            pushToArmSensorQ();
            break;

        case ROVER:
            pushToRoverQ();
            break;

        case ROVER_SENSOR:
            pushToRoverSensorQ();
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

