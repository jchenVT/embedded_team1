/*
 *  prox1read.c
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#include "prox1read.h"

/*
 *  @function   readProximitySensor
 *              Main thread that will perform a blocking read on
 *              the UART0 pin to read the proximity sensor 1 data.
 *
 *  @params     arg0
 *  @return     void
 */
void *readProximity1Thread(void *arg0) {

    dbgOutputLoc(INIT_TIMER1);

    /* One-time initialization of software timer */
    TimerHandle_t timer10ms = xTimerCreate("10ms", pdMS_TO_TICKS(10), pdTRUE, NULL, timerCallback);
    xTimerStart(timer10ms, 0);

    /* Initialize pin to read proximity 1 */
    GPIO_setConfig(CONFIG_GPIO_PROX1, GPIO_CFG_INPUT);
}

/*
 *  @function   timerCallback
 *              Adds the current sensor reading to the sensor queue.
 *
 *  @params     xTimer
 *  @return     void
 */
void timerCallback(TimerHandle_t xTimer) {

    dbgOutputLoc(TIMER1_CALLBACK);

    /* Check pin and send to queue */
    int reading = GPIO_read(CONFIG_GPIO_PROX1);

    /* send to the correct sensor queue */
    dbgOutputLoc(SEND_PROX1Q);
    sendToProx1Q(reading);
}
