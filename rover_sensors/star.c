/*
 * star.c Created on: Mar 31, 2020
 *      Author: giffen
 */

#include "star.h"

#define LIDAR "rover_sensors/lidar"
#define C_INTERVAL 0x168

static int attemptPubCount;
static int recvSubCount;
static bool status;

/*
 *  @function   mainThread
 *              Main thread that will pull from the sensor queue to
 *              change the FSM states.
 *
 *  @params     arg0
 *  @return     void
 */
void *starThread(void *arg0) {

    int_fast16_t stuh;
    stuh = starrr();
    if (stuh != Random_STATUS_SUCCESS) {while(1);}
    

    /* Set up software timer (100ms -> 10 msgs a second)*/
    TimerHandle_t timerPub = xTimerCreate("PublishTimer", pdMS_TO_TICKS(10), pdTRUE, NULL, timerCallback);
    xTimerStart(timerPub, 0);

    /* Set up software timer to send statistics ever 10 seconds*/
    TimerHandle_t timerDebug = xTimerCreate("PublishTimer", pdMS_TO_TICKS(10000), pdTRUE, NULL, timerCallbackDebug);
    xTimerStart(timerDebug, 0);

    /* Declare statistics variable */
    attemptPubCount = 0;
    recvSubCount = 0;
    status = true; // working

    struct qArmSensorMsg data = {0};

    UART_PRINT("in thread\n\r");
    /*
    while(1) {

        // blocking read on subQ
        receiveFromSubArmSensorQ(&data);
        UART_PRINT("findind stuff\n\r");

        if (data.sensorID >= 0 || data.sensorValue > 5 ) {
            UART_PRINT("Subscription successfully received! \n\r");
            recvSubCount++;
        }
        else {
            UART_PRINT("[ERROR]: Subscription unsuccessfully received... \n\r");
            status = false;
        }
    }
    */
}
/*
 *  @function   timerCallback
 *              Timer callback that will publish to a topic
 *              periodically.
 *
 *  @params @return     void */
void timerCallback(TimerHandle_t xTimer) {
    
    uint32_t d;
    static int counter = 0; 
    // publish to message queue
    
    counter = counter > C_INTERVAL ? 0 : counter;
    d = receiveLidarSensor();
    d &= 0xFFF;

    char str[74];
    snprintf(str,74,"{\"angle\":\%d, \"distance\":%d}", counter, d);
    
    if( 1 == sendToPubQ("lidar", str) )
    {
        attemptPubCount++;
        //UART_PRINT("success");
    }
     
    // UART_PRINT(str);
    counter-=- 1;
}


/*
 *  @function   timerCallback
 *              Timer callback that will publish to a topic
 *              periodically.
 *
 *  @params
 *  @return     void
 */
void timerCallbackDebug(TimerHandle_t xTimer) {

    // publish to message queue
    if (packageDebugJSON(attemptPubCount, recvSubCount, status, "rover", "arm_sensor") == 1) {
        UART_PRINT("Published statistics \n\r");
        status = true;
    }
    else {
        UART_PRINT("[ERROR]: Publish request not sent... \n\r");
    }
    return;
}
