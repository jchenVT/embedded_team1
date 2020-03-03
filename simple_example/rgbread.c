/*
 *  rgbread.c
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#include "rgbread.h"

I2C_Handle i2cHandle;

/*
 *  @function   readRGBThread()
 *              Main thread that will perform a blocking read on
 *              the UART0 pin to read the proximity sensor 1 data.
 *
 *  @params     arg0
 *  @return     void
 */
void *readRGBThread(void *arg0) {

    /* Initialize optional I2C bus parameters */
    I2C_Params params;
    I2C_Params_init(&params);
    params.bitRate = I2C_400kHz;
    params.transferMode = I2C_MODE_BLOCKING;

    /* Open I2C bus for usage */
    I2C_init();
    i2cHandle = I2C_open(CONFIG_I2C_0, &params);
    if (i2cHandle == NULL) {
        stop_all(FAILED_I2C_INIT);
    }
    else {
        dbgOutputLoc(INIT_I2C);
    }

    /* Initialize RGB sensor enable register */
    I2C_Transaction transactionEnable = {0};
    transactionEnable.slaveAddress = OPT_ADDR;
    uint8_t txBufferEnable[3] = {0x00, 0x01, 0x03};
    transactionEnable.writeBuf = txBufferEnable;
    transactionEnable.writeCount = 3;
    transactionEnable.readBuf = NULL;
    transactionEnable.readCount = 0;
    if (I2C_transfer(i2cHandle, &transactionEnable)) {
        dbgOutputLoc(RECV_RGBQREAD);
    }

    /* One-time initialization of software timer */
    TimerHandle_t timerRGB = xTimerCreate("RGB", pdMS_TO_TICKS(100), pdTRUE, NULL, timerRGBCallback);
    xTimerStart(timerRGB, 0);
}

/*
 *  @function   timerCallback
 *              Adds the current sensor reading to the sensor queue.
 *
 *  @params     xTimer
 *  @return     void
 */
void timerRGBCallback(TimerHandle_t xTimer) {

    dbgOutputLoc(TIMER_CALLBACK);

    I2C_Transaction transaction = {0};
    uint8_t txBuffer[6] = {0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B}; // r, g, b
    uint8_t rxBuffer[6] = {0};

    /* Setup data transfer for the three channels */
    transaction.slaveAddress = OPT_ADDR;
    transaction.writeBuf = txBuffer;
    transaction.writeCount = 6;
    transaction.readBuf = rxBuffer;
    transaction.readCount = 6;

    /* Read from I2C slave device */
    dbgOutputLoc(WAIT_RGBQREAD);
    if (I2C_transfer(i2cHandle, &transaction)) {
        dbgOutputLoc(RECV_RGBQREAD);

        /* Combine higher and lower bit readings */
        uint16_t r = rxBuffer[1];
        r = (r << 8) + rxBuffer[0];
        uint16_t g = rxBuffer[3];
        g = (g << 8) + rxBuffer[2];
        uint16_t b = rxBuffer[5];
        b = (b << 8) + rxBuffer[4];

        /* Output the RGB values for color channel */
        int i;
        char sensorOut[16];
        int size = snprintf(sensorOut, 32, "R=%d", r);
        for (i=0; i<size; i++) {
            dbgUARTVal(sensorOut[i]);
        }

        size = snprintf(sensorOut, 32, " G=%d", g);
        for (i=0; i<size; i++) {
            dbgUARTVal(sensorOut[i]);
        }

        size = snprintf(sensorOut, 32, " B=%d", b);
        for (i=0; i<size; i++) {
            dbgUARTVal(sensorOut[i]);
        }

        /* Find most prominent color */
        if (r > 10000) {
            dbgUARTVal('R');
        }
        if (g > 10000) {
            dbgUARTVal('G');
        }
        if (b > 10000) {
            dbgUARTVal('B');
        }

        dbgUARTVal(' ');
    }
    else {
        stop_all(FAILED_I2C_CALLBACK);
    }
}
