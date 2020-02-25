/*
 * timerone.c
 *
 *  Created on: Feb 20, 2020
 *      Author: trieu
 */

#include <rover_spi.h>

static char clearCountBuffer[1] = {0xE0};
static char clearDTRBuffer[5] = {0x98, 0x00, 0x00, 0x00, 0x00};
static char initBuffer[2] = {0x88, 0x03};
static char readTxBuffer[SPI_MSG_LENGTH] = {0x60, 0x00, 0x00, 0x00, 0x00};
static char readRxBuffer[SPI_MSG_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00};

static SPI_Handle masterSpi;
static SPI_Transaction transaction;
static TimerHandle_t spiTimer_Handle;

void spi_setup() {

    /*****************************/
    dbgOutputLoc(SPI_INITIALIZE);
    /*****************************/

    /* Call driver init functions. */
    SPI_init();
    SPI_Params      spiParams;

    /* Set Master as an output and slaves as inputs */
    GPIO_setConfig(CONFIG_SPI_MASTER_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_SPI_SLAVE130_READY, GPIO_CFG_INPUT);
    GPIO_setConfig(CONFIG_SPI_SLAVE128_READY, GPIO_CFG_INPUT);
    GPIO_setConfig(CONFIG_SPI_SLAVE129_READY, GPIO_CFG_INPUT);
    /*
     * Handshake - Set CONFIG_SPI_MASTER_READY high to indicate master is ready
     * to run.  Wait CONFIG_SPI_SLAVE_READY to be high.
     */
    GPIO_write(CONFIG_SPI_MASTER_READY, 1);
    GPIO_write(CONFIG_SPI_SLAVE128_READY, 1);
    GPIO_write(CONFIG_SPI_SLAVE129_READY, 1);
    GPIO_write(CONFIG_SPI_SLAVE130_READY, 1);

    /* Handshake complete; now configure interrupt on CONFIG_SPI_SLAVE_READY */
    GPIO_setConfig(CONFIG_SPI_SLAVE128_READY, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setConfig(CONFIG_SPI_SLAVE129_READY, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setConfig(CONFIG_SPI_SLAVE130_READY, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    /* Open SPI as master (default) */
    SPI_Params_init(&spiParams);
    spiParams.mode = SPI_MASTER;
    spiParams.frameFormat = SPI_POL0_PHA1;
    spiParams.bitRate = 9600;
    spiParams.dataSize = 8;

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

    spiTimer_Handle = xTimerCreate("spi_timer", pdMS_TO_TICKS(TIMER_LENGTH), pdTRUE, (void*)0, spitimerCallback);

    if (spiTimer_Handle == NULL) {
        stop_all(FAIL_TIMER_INIT);
    }

    /*****************************/
    dbgOutputLoc(SPI_TIMER_OPENING);
    /*****************************/

    if (xTimerStart(spiTimer_Handle, 0) != pdPASS) {
        stop_all(FAIL_TIMER_START);
    }
}

void spi_close() {

    dbgOutputLoc(SPI_CLOSING);

    SPI_close(masterSpi);
    xTimerStop(spiTimer_Handle, 0);
}

void spitimerCallback(TimerHandle_t myHandle) {
    sendMsgToEncoderQ();
}

void readEncoder(int encoder) {
    memset((void *) readRxBuffer, 0, SPI_MSG_LENGTH);
    transaction.count = SPI_MSG_LENGTH;
    transaction.txBuf = (void*)readTxBuffer;
    transaction.rxBuf = (void*)readRxBuffer;

    if (encoder == e128) {
        if (!transferData(CONFIG_SPI_SLAVE128_READY)) {
            stop_all(FAIL_SPI_READING_128);
        }
    }
    else if (encoder == e129) {
        if (!transferData(CONFIG_SPI_SLAVE129_READY)) {
            stop_all(FAIL_SPI_READING_129);
        }
    }
    else if (encoder == e130) {
        if (!transferData(CONFIG_SPI_SLAVE130_READY)) {
            stop_all(FAIL_SPI_READING_130);
        }
    }

    long data;
    int i;
    for (i=0;i<sizeof(readRxBuffer); i++) {
        data = (data << 8) + readRxBuffer[i];
    }

    if (sendMsgToReceiveQ(false, data, encoder) != pdPASS) {
        stop_all(FAIL_SPI_SEND_TO_Q);
    }
}

void initEncoders() {
    memset((void *) readRxBuffer, 0, SPI_MSG_LENGTH);
    transaction.count = 2;
    transaction.txBuf = (void*)initBuffer;
    transaction.rxBuf = (void*)readRxBuffer;

    dbgOutputLoc(SPI_ENCODER_INIT);

    if (!transferData(CONFIG_SPI_SLAVE128_READY)) {
        stop_all(FAIL_SPI_READING_128);
    }
    if (!transferData(CONFIG_SPI_SLAVE129_READY)) {
        stop_all(FAIL_SPI_READING_129);
    }
    if (!transferData(CONFIG_SPI_SLAVE130_READY)) {
        stop_all(FAIL_SPI_READING_130);
    }
}

void clearEncoderCounts() {
    memset((void *) readRxBuffer, 0, SPI_MSG_LENGTH);
    transaction.count = 5;
    transaction.txBuf = (void*)clearDTRBuffer;
    transaction.rxBuf = (void*)readRxBuffer;

    dbgOutputLoc(SPI_ENCODER_CLEARING);

    if (!transferData(CONFIG_SPI_SLAVE128_READY)) {
        stop_all(FAIL_SPI_READING_128);
    }
    if (!transferData(CONFIG_SPI_SLAVE129_READY)) {
        stop_all(FAIL_SPI_READING_129);
    }
    if (!transferData(CONFIG_SPI_SLAVE130_READY)) {
        stop_all(FAIL_SPI_READING_130);
    }

    memset((void *) readRxBuffer, 0, SPI_MSG_LENGTH);
    transaction.count = 1;
    transaction.txBuf = (void*)clearCountBuffer;
    transaction.rxBuf = (void*)readRxBuffer;

    dbgOutputLoc(SPI_ENCODER_CLEARING);

    if (!transferData(CONFIG_SPI_SLAVE128_READY)) {
        stop_all(FAIL_SPI_READING_128);
    }
    if (!transferData(CONFIG_SPI_SLAVE129_READY)) {
        stop_all(FAIL_SPI_READING_129);
    }
    if (!transferData(CONFIG_SPI_SLAVE130_READY)) {
        stop_all(FAIL_SPI_READING_130);
    }
}

bool transferData(int encoder) {

    dbgOutputLoc(SPI_SENDING);

    GPIO_write(encoder, 0);

    bool transferOK = SPI_transfer(masterSpi, &transaction);

    GPIO_write(encoder, 1);
    return transferOK;
}

void *spiThread(void *arg0) {

    spi_setup();

    initEncoders();
    clearEncoderCounts();
    timer_setup();

    bool temp = false;

    while(1) {
        receiveFromEncoderQ(&temp);

        dbgOutputLoc(SPI_READING_128);
        readEncoder(e128);

        dbgOutputLoc(SPI_READING_129);
        readEncoder(e129);

        dbgOutputLoc(SPI_READING_130);
        readEncoder(e130);

        clearEncoderCounts();
    }

}
