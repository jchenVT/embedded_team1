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

    dbgOutputLoc(INIT_I2C);

    I2C_init();

    /* Initialize optional I2C bus parameters */
    I2C_Params params;
    I2C_Params_init(&params);
    params.bitRate = I2C_400kHz;
    params.transferMode = I2C_MODE_CALLBACK;
    params.transferCallbackFxn = i2cCallback;

    /* Open I2C bus for usage */
    I2C_Handle i2cHandle = I2C_open(SENSORS, &params);

    /* Initialize slave address of transaction */
    I2C_Transaction transaction = {0};
    transaction.slaveAddress = OPT_ADDR;

    /* Read from I2C slave device */
    I2C_transfer(i2cHandle, &transaction);
}

/*
 *  @function   i2cCallback()
 *              Callback for when the I2C is done writing.
 *
 *  @params     I2C_Handle handle, I2C_Transaction *msg, bool status
 *  @return     void
 */
void i2cCallback(I2C_Handle handle, I2C_Transaction *msg, bool status) {

    dbgOutputLoc(I2C_CALLBACK);

    if (status == false) {
        stop_all(FAILED_I2C_CALLBACK);
    }

    // Check for a semaphore handle
    if (msg->arg != NULL) {
        // Perform a semaphore post
        //sem_post((sem_t *) (msg->arg));
    }
    else {
        stop_all(FAILED_RGB_BADVAL);
    }
}
