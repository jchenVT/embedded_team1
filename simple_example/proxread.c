/*
 *  proxread.c
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#include <proxread.h>


/*
 *  @function   readProximitySensor
 *              Main thread that will perform a blocking read on
 *              the UART0 pin to read the proximity sensor 1 data.
 *
 *  @params     arg0
 *  @return     void
 */
void *readProximityThread(void *arg0) {

    dbgOutputLoc(INIT_TIMER);

    /* One-time initialization of software timer */
    TimerHandle_t timerProx = xTimerCreate("PROX", pdMS_TO_TICKS(50), pdTRUE, NULL, timerCallback);
    xTimerStart(timerProx, 0);

    /* Initialize pin to read proximity sensors */
    GPIO_setConfig(CONFIG_GPIO_PROX1, GPIO_CFG_INPUT);
    GPIO_setConfig(CONFIG_GPIO_PROX2, GPIO_CFG_INPUT);
}

/*
 *  @function   timerCallback
 *              Adds the current sensor reading to the sensor queue.
 *
 *  @params     xTimer
 *  @return     void
 */
void timerCallback(TimerHandle_t xTimer) {

    dbgOutputLoc(TIMER_CALLBACK);

    /* Check pin and send to queue */
    int reading1 = GPIO_read(CONFIG_GPIO_PROX1);
    int reading2 = GPIO_read(CONFIG_GPIO_PROX2);

    /* Print to UART for Debug */
    dbgUARTVal('P');
    dbgUARTVal('1');
    dbgUARTVal(':');
    if (reading1 == 0) {
        dbgUARTVal('0');
    }
    else if (reading1 == 1) {
        dbgUARTVal('1');
    }
    else {
        stop_all(FAILED_PROX1_BADVAL);
    }
    dbgUARTVal(' ');

    /* Send to the correct sensor queue */
    dbgOutputLoc(SEND_PROX1Q);
    sendProxToSensorQ(PROX1_DATA, reading1);

    /* Print to UART for Debug */
    dbgUARTVal('P');
    dbgUARTVal('2');
    dbgUARTVal(':');
    if (reading2 == 0) {
        dbgUARTVal('0');
    }
    else if (reading2 == 1) {
        dbgUARTVal('1');
    }
    else {
        stop_all(FAILED_PROX2_BADVAL);
    }
    dbgUARTVal(' ');

    /* Send to the correct sensor queue */
    dbgOutputLoc(SEND_PROX2Q);
    sendProxToSensorQ(PROX2_DATA, reading2);
}
