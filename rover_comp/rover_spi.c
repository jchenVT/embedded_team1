/*
 * timerone.c
 *
 *  Created on: Feb 20, 2020
 *      Author: trieu
 */

#include <rover_spi.h>

static unsigned char masterRxBuffer[SPI_MSG_LENGTH];
static unsigned char masterTxBuffer[SPI_MSG_LENGTH] = {0x60, 0x00, 0x00, 0x00, 0x00};

static SPI_Handle masterSpi;
static TimerHandle_t timer0;

void spi_setup() {

    /*****************************/
    dbgOutputLoc(SPI_INITIALIZE);
    /*****************************/

    /* Call driver init functions. */
    SPI_init();
    SPI_Params      spiParams;

    /*
     * Handshake - Set CONFIG_SPI_MASTER_READY high to indicate master is ready
     * to run.  Wait CONFIG_SPI_SLAVE_READY to be high.
     */
    GPIO_write(CONFIG_SPI_MASTER_READY, 1);
    while (GPIO_read(CONFIG_SPI_SLAVE128_READY) == 0) {}
    while (GPIO_read(CONFIG_SPI_SLAVE129_READY) == 0) {}
    while (GPIO_read(CONFIG_SPI_SLAVE130_READY) == 0) {}

    /* Handshake complete; now configure interrupt on CONFIG_SPI_SLAVE_READY */
    GPIO_setConfig(CONFIG_SPI_SLAVE128_READY, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setConfig(CONFIG_SPI_SLAVE129_READY, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setConfig(CONFIG_SPI_SLAVE130_READY, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    /* Open SPI as master (default) */
    SPI_Params_init(&spiParams);
    spiParams.mode = SPI_MASTER;
    spiParams.transferMode = SPI_MODE_BLOCKING;
    spiParams.frameFormat = SPI_POL0_PHA1;
    spiParams.bitRate = 9600;
    spiParams.dataSize = 4; // UNSURE

    /*****************************/
    dbgOutputLoc(SPI_OPENING);
    /*****************************/

    masterSpi = SPI_open(CONFIG_SPI_MASTER, &spiParams);
    if (masterSpi == NULL) {
        /*****************************/
        stop_all(FAIL_SPI_INIT);
        /*****************************/
    }

}

void timer_setup() {

    /*****************************/
    dbgOutputLoc(SPI_TIMER_INITIALIZE);
    /*****************************/

    timer0 = xTimerCreate("spi_timer", pdMS_TO_TICKS(TIMER_LENGTH), pdTRUE, NULL, timerCallback);

    if (timer0 == NULL {
        stop_all(FAIL_TIMER_INIT);
    }

    /*****************************/
    dbgOutputLoc(SPI_TIMER_OPENING);
    /*****************************/

    if (xTimerStart(timer0, 0) != pdPASS) {
        stop_all(FAIL_TIMER_START);
    }

    GPIO_write(CONFIG_SPI_MASTER_READY, 0);
}

void spi_close() {
    SPI_close(masterSpi);
    xTimerStop(timer0);

    /* Example complete - set pins to a known state */
    GPIO_setConfig(CONFIG_SPI_SLAVE128_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_SPI_SLAVE129_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_SPI_SLAVE130_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_SPI_MASTER_READY, 0);
}

void timerCallback(TimerHandle_t myHandle) {
    /*****************************/
    dbgOutputLoc(SPI_READING_128);
    /*****************************/
    //readEncoder(e128);
    /*****************************/
    dbgOutputLoc(SPI_READING_129);
    /*****************************/
    //readEncoder(e129);
    /*****************************/
    dbgOutputLoc(SPI_READING_130);
    /*****************************/
    //readEncoder(e130);
}

void readEncoder(int encoder) {

    if (encoder == e128) {
        GPIO_write(CONFIG_SPI_SLAVE128_READY, 0);
        if (!transferData(encoder)) {
            stop_all(FAIL_SPI_READING_128);
        }
        GPIO_write(CONFIG_SPI_SLAVE128_READY, 1);
    }
    else if (encoder == e129) {
        GPIO_write(CONFIG_SPI_SLAVE129_READY, 0);
        if (!transferData(encoder)) {
            stop_all(FAIL_SPI_READING_129);
        }
        GPIO_write(CONFIG_SPI_SLAVE129_READY, 1);
    }
    else if (encoder == e130) {
        GPIO_write(CONFIG_SPI_SLAVE130_READY, 0);
        if (!transferData(encoder)) {
            stop_all(FAIL_SPI_READING_130);
        }
        GPIO_write(CONFIG_SPI_SLAVE130_READY, 1);
    }
}

bool transferData(int encoder) {

    static SPI_Transaction transaction;

    memset((void *) masterRxBuffer, 0, SPI_MSG_LENGTH);
    transaction.count = SPI_MSG_LENGTH;
    transaction.txBuf = (void *) masterTxBuffer;
    transaction.rxBuf = (void *) masterRxBuffer;

    bool transferOK = SPI_transfer(masterSpi, &transaction);
    if (transferOK) {
        long data = 0;
        int i;
        for (i=0;i<SPI_MSG_LENGTH; i++) {
            data = (data << 8) | masterRxBuffer[i];
        }

        sendMsgToReceiveQ(false, data, encoder);
    }

    return transferOK;
}
