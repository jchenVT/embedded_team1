/*
 *  sensor_read.c
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#include <sensor_read.h>
#include <stdlib.h>

I2C_Handle i2cHandle;

static int attemptPubCount;
static int recvSubCount;
static bool status;

/*
 *  @function   readProximitySensor
 *              Main thread that will perform a blocking read on
 *              the UART0 pin to read the proximity sensor 1 data.
 *
 *  @params     arg0
 *  @return     void
 */
void *readSensorsThread(void *arg0) {

    /* Initialize pin to read proximity sensors */
    GPIO_setConfig(CONFIG_GPIO_PROX1, GPIO_CFG_INPUT);
    GPIO_setConfig(CONFIG_GPIO_PROX2, GPIO_CFG_INPUT);

    /* Initialize optional I2C bus parameters */
    I2C_Params params;
    I2C_Params_init(&params);
    params.bitRate = I2C_400kHz;
    params.transferMode = I2C_MODE_BLOCKING;

    /* Open I2C bus for usage */
    I2C_init();
    i2cHandle = I2C_open(CONFIG_I2C_0, &params);
    if (i2cHandle == (void *)0) {
        stop_all();
    }

    /* Initialize RGB sensor enable register */
    I2C_Transaction transactionEnable = {0};
    transactionEnable.slaveAddress = OPT_ADDR;
    uint8_t txBufferEnable[3] = {0x00, 0x01, 0x03};
    transactionEnable.writeBuf = txBufferEnable;
    transactionEnable.writeCount = 3;
    transactionEnable.readBuf = (void *)0;
    transactionEnable.readCount = 0;
    if (I2C_transfer(i2cHandle, &transactionEnable)) {

    }

    /* Initialize debug stats */
    attemptPubCount = 0;
    recvSubCount = 0;
    status = true;

    /* One-time initialization of software timer */
    TimerHandle_t timerStart= xTimerCreate("STRT", pdMS_TO_TICKS(50), pdTRUE, (void *)0, timerStartCallback);
    TimerHandle_t timerProx = xTimerCreate("PROX", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, timerProxCallback);
    TimerHandle_t timerRGB = xTimerCreate("RGB", pdMS_TO_TICKS(2000), pdTRUE, (void *)0, timerRGBCallback);
    TimerHandle_t timerDebug = xTimerCreate("DEBUG", pdMS_TO_TICKS(5000), pdTRUE, (void *)0, timerDebugCallback);
    xTimerStart(timerStart, 0);
    xTimerStart(timerDebug, 0);

    /* Command queue data to be changed */
    struct qCommandMsg data = {0, 0, 0};

    /* State that will be used for pausing and resuming states */
    int state = OFF;

    while (1) {

        receiveFromSubCommandQ(&data);

        recvSubCount++;

        /* Check if there is an error */
        if (data.error == 9) {
            xTimerStop(timerProx, 0);
            xTimerStop(timerRGB, 0);
            xTimerStop(timerDebug, 0);
            packageErrorJSON(3);
        }
        else if (data.error > 0) {
            char b[50];
            int j = snprintf(b, 50, "[ERROR] - Stopping due to server error: %d", data.error);
            UART_PRINT(b);
            stop_all();
        }


        /* Change timer state based on the msg given */
        switch (state) {

        case OFF:
            if (data.message == ON) {
                xTimerStart(timerProx, 0);
                xTimerStart(timerRGB, 0);
                //xTimerStop(timerStart, 0);

                state = ON;
            }
            break;

        case ON:
            if (data.message == PAUSE) {
                xTimerStop(timerProx, 0);
                xTimerStop(timerRGB, 0);

                state = PAUSE;
            }
            break;

        case PAUSE:
            if (data.message == ON) {
                xTimerStart(timerProx, 0);
                xTimerStart(timerRGB, 0);

                state = ON;
            }
            break;
        }
    }
}

/*
 *  @function   timerStartCallback
 *              Does nothing. Allows for MQTT to initialize correctly.
 *
 *  @params     xTimer
 *  @return     void
 */
void timerStartCallback(TimerHandle_t xTimer) {

    /* DO NOTHING */
}

/*
 *  @function   timerDebugCallback
 *              Sends MQTT information periodically.
 *
 *  @params     xTimer
 *  @return     void
 */
void timerDebugCallback(TimerHandle_t xTimer) {

    /* DO NOTHING */
    packageDebugJSON(attemptPubCount, recvSubCount, status, "command", "arm_sensor");

    /* Reset status variables */
    status = true;
}

/*
 *  @function   timerProxCallback
 *              Adds the current prox sensor reading to the sensor queue.
 *
 *  @params     xTimer
 *  @return     void
 */
void timerProxCallback(TimerHandle_t xTimer) {

    /* Check pin and send to queue */
    int reading1 = GPIO_read(CONFIG_GPIO_PROX1);
    int reading2 = GPIO_read(CONFIG_GPIO_PROX2);

    /* Send to the correct sensor queue */
    if (sendProxToSensorQ(PROX1_DATA, reading1) == 1) {
        attemptPubCount++;
    }
    else {
        status = false;
    }

    /* Send to the correct sensor queue */
    if (sendProxToSensorQ(PROX2_DATA, reading2) == 1) {
        attemptPubCount++;
    }
    else {
        status = false;
    }
}

/*
 *  @function   timerRGBCallback
 *              Adds the current RGB sensor reading to the sensor queue.
 *
 *  @params     xTimer
 *  @return     void
 */
void timerRGBCallback(TimerHandle_t xTimer) {

    I2C_Transaction transaction = {0};
    uint8_t txBuffer[8] = {0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B}; // r, g, b, c
    uint8_t rxBuffer[8] = {0};

    /* Setup data transfer for the three channels */
    transaction.slaveAddress = OPT_ADDR;
    transaction.writeBuf = txBuffer;
    transaction.writeCount = 8;
    transaction.readBuf = rxBuffer;
    transaction.readCount = 8;

    /* Read from I2C slave device */
    if (I2C_transfer(i2cHandle, &transaction)) {

        /* Combine higher and lower bit readings */
        uint16_t c = rxBuffer[1];
        c = (c << 8) + rxBuffer[0];
        uint16_t r = rxBuffer[3];
        r = (r << 8) + rxBuffer[2];
        uint16_t g = rxBuffer[5];
        g = (g << 8) + rxBuffer[4];
        uint16_t b = rxBuffer[7];
        b = (b << 8) + rxBuffer[6];

        /* Normalize each reading based on clear bit */
        r = (r*256) / c;
        g = (g*256) / c;
        b = (b*256) / c;

        /* Send to RGB message queue */
        if (sendRGBToSensorQ(r, g, b) == 1) {
            attemptPubCount++;
        }
        else {
            status = false;
        }
    }
    else {
        status = false;
        //stop_all();
    }
}
