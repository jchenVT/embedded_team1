/*
 *  rgbread.c
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#include "rgbread.h"

/* Global semaphore handle for callback */
sem_t semaphoreHandle;

/* Buffer to push readings into */
uint8_t rxBuffer[2];

/*
 *  @function   readRGBThread()
 *              Main thread that will perform a blocking read on
 *              the UART0 pin to read the proximity sensor 1 data.
 *
 *  @params     arg0
 *  @return     void
 */
void *readRGBThread(void *arg0) {

    dbgOutputLoc(INIT_I2C);

    I2C_init();
    sem_init(&semaphoreHandle, 0, 0);

    /* Initialize optional I2C bus parameters */
    I2C_Params params;
    I2C_Params_init(&params);
    params.bitRate = I2C_100kHz;
    params.transferMode = I2C_MODE_CALLBACK;
    params.transferCallbackFxn = i2cCallback;

    /* Open I2C bus for usage */
    I2C_Handle i2cHandle = I2C_open(CONFIG_I2C_0, &params);

    /* Initialize slave address of transaction */
    I2C_Transaction transaction = {0};
    transaction.slaveAddress = OPT_ADDR;
    transaction.arg = NULL;

    while (1) {

        /* Read from I2C slave device */
        I2C_transfer(i2cHandle, &transaction);

        /* Waits for post to continue thread */
        sem_wait(&semaphoreHandle);
    }
}

/*
 *  @function   i2cCallback()
 *              Callback for when the I2C is done writing.
 *
 *  @params     handle, msg, status
 *  @return     void
 */
void i2cCallback(I2C_Handle handle, I2C_Transaction *msg, bool status) {

    dbgOutputLoc(I2C_CALLBACK);

    if (status == false) {
        stop_all(FAILED_I2C_CALLBACK);
    }

    if (msg->arg != NULL) {

        /* Check if message contents are okay */


        /* Send to RGB queue */
        dbgOutputLoc(SEND_RGBQ);
        //sendToRGBQ(value);

        /* Perform a semaphore post to signal complete */
        sem_post((sem_t *) (msg->arg));
    }
    else {
        stop_all(FAILED_RGB_BADVAL);
    }
}
