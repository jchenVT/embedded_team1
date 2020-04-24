/*
 *  proxread.c
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#include <proxread.h>
#include <stdlib.h>

/*
 *  @function   readProximitySensor
 *              Main thread that will perform a blocking read on
 *              the UART0 pin to read the proximity sensor 1 data.
 *
 *  @params     arg0
 *  @return     void
 */
void *readProximityThread(void *arg0) {

    /* Initialize pin to read proximity sensors */
    GPIO_setConfig(CONFIG_GPIO_PROX1, GPIO_CFG_INPUT);
    GPIO_setConfig(CONFIG_GPIO_PROX2, GPIO_CFG_INPUT);

    /* One-time initialization of software timer */
    TimerHandle_t timerProx = xTimerCreate("PROX", pdMS_TO_TICKS(50), pdTRUE, (void *)0, timerCallback);
    xTimerStart(timerProx, 0);

    /* Command queue data to be changed */
    struct qCommandMsg data = {0, 0, 0};

    while (1) {

        receiveFromSubCommandQ(&data);
        UART_PRINT("MSG_RECV");
    }
}

/*
 *  @function   timerCallback
 *              Adds the current sensor reading to the sensor queue.
 *
 *  @params     xTimer
 *  @return     void
 */
void timerCallback(TimerHandle_t xTimer) {

    /* Check pin and send to queue */
    int reading1 = GPIO_read(CONFIG_GPIO_PROX1);
    int reading2 = GPIO_read(CONFIG_GPIO_PROX2);

    /* Send to the correct sensor queue */
    sendProxToSensorQ(PROX1_DATA, reading1);

    /* Send to the correct sensor queue */
    sendProxToSensorQ(PROX2_DATA, reading2);
}
