/*
 *  rgbread.c
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#include "rgbread.h"

/*
 *  @function   readRGBThread()
 *              Main thread that will perform a blocking read on
 *              the UART0 pin to read the proximity sensor 1 data.
 *
 *  @params     arg0
 *  @return     void
 */
void *readRGBThread(void *arg0) {

    //sem_init(&semaphoreHandle, 0, 0);

    /* Initialize optional I2C bus parameters */
    I2C_Params params;
    I2C_Params_init(&params);
    params.bitRate = I2C_400kHz;
    params.transferMode = I2C_MODE_BLOCKING;
    //params.transferCallbackFxn = i2cCallback;

    /* Open I2C bus for usage */
    I2C_init();
    I2C_Handle i2cHandle = I2C_open(CONFIG_I2C_0, &params);
    if (i2cHandle == NULL) {
        stop_all(FAILED_I2C_INIT);
    }
    else {
        dbgOutputLoc(INIT_I2C);
    }

    /* Initialize slave address of transaction */
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


    /* Setup data transfer */
    I2C_Transaction transaction = {0};
    transaction.slaveAddress = OPT_ADDR;
    uint8_t txBuffer[3] = {0x97, 0x99, 0x9B};
    uint8_t rxBuffer[3] = {0};
    transaction.writeBuf = txBuffer;
    transaction.writeCount = 3;
    transaction.readBuf = rxBuffer;
    transaction.readCount = 3;

    while (1) {

        /* Read from I2C slave device */
        if (I2C_transfer(i2cHandle, &transaction)) {
            dbgOutputLoc(RECV_RGBQREAD);
        }
        else {
            stop_all(FAILED_I2C_CALLBACK);
        }
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
        //sem_post((sem_t *) (msg->arg));
    }
    else {
        stop_all(FAILED_RGB_BADVAL);
    }
}
