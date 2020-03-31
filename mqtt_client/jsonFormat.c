#include <stdio.h>
#include <mqtt_queue.h>
#include <stdbool.h>

int packageArmJSON(int state) {
    char json[20];
    snprintf(json, 20, "{ \"state\": %d }", state);
//    return sendToPubQ(0, &json);
    return 1;
}

int packageArmSensorJSON(int sensorID, int sensorValue) {
    char json[50];
    snprintf(json, 50, "{ \"sensorID\": %d, \"sensorValue\": %d }", sensorID, sensorValue);
//    return sendToPubQ(1, &json);
    return 1;
}

int packageRoverJSON(int state) {
    char json[20];
    snprintf(json, 20, "{ \"state\": %d }", state);
//    return sendToPubQ(2, &json);
    return 1;
}

int packageRoverSensorJSON(bool move_to_point, int point_x, int point_y, int angle_rotate) {
    char json[110];
    snprintf(json, 120, "{ \"move_to_point\": %s, \"point_x\": %d, \"point_y\": %d, \"angle_rotate\": %d }", move_to_point ? "true"; "false", point_x, point_y, angle_rotate);
//    return sendToPubQ(3, &json);
    return 1;
}

