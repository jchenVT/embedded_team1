/*
 * timerone.c
 *
 *  Created on: Feb 20, 2020
 *      Author: trieu
 */

#include <rover_spi.h>

static char clearCountBuffer[1] = {0x20};
static char clearDataBuffer[5] = {0x98, 0x00, 0x00, 0x00, 0x00};
static char setDataToCounter[1] = {0xE0};
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

    /* Set slave controllers as outputs */
    GPIO_setConfig(CONFIG_SPI_SLAVE130_READY, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_SPI_SLAVE128_READY, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_SPI_SLAVE129_READY, GPIO_CFG_OUTPUT);

    /* Set slaves high (high->low is Slave Select) */
    GPIO_write(CONFIG_SPI_SLAVE128_READY, 1);
    GPIO_write(CONFIG_SPI_SLAVE129_READY, 1);
    GPIO_write(CONFIG_SPI_SLAVE130_READY, 1);

    SPI_Params_init(&spiParams);
    spiParams.mode = SPI_MASTER;
    spiParams.frameFormat = SPI_POL0_PHA0;
    spiParams.bitRate = 10000;
    spiParams.dataSize = 8;

    dbgOutputLoc(SPI_OPENING);

    masterSpi = SPI_open(CONFIG_SPI_0, &spiParams);
    if (masterSpi == NULL) {
        stop_all(FAIL_SPI_INIT);
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

    dbgOutputLoc(SPI_DATA);
    long data = 0;
    int i;
    for (i=1;i<sizeof(readRxBuffer); i++) {
        data = (data << 8) + readRxBuffer[i];
        dbgOutputLoc(readRxBuffer[i]);
    }

    if (data < 5) {
        dbgOutputLoc(0xF0);
    }
    else {
        dbgOutputLoc(0xF1);
    }

    dbgOutputLoc(SPI_DATA);

    if (sendMsgToReceiveQ(false, false, 0, (double)data, (double)encoder) != pdPASS) {
        stop_all(FAIL_SPI_SEND_TO_Q);
    }
}

void initEncoders() {
    transaction.count = 2;
    transaction.txBuf = (void*)initBuffer;
    transaction.rxBuf = NULL;

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
    transaction.count = 1;
    transaction.txBuf = (void*)clearCountBuffer;
    transaction.rxBuf = NULL;

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

//    transaction.count = 1;
//    transaction.txBuf = (void*)setDataToCounter;
//    transaction.rxBuf = NULL;
//
//    dbgOutputLoc(SPI_ENCODER_CLEARING);
//
//    if (!transferData(CONFIG_SPI_SLAVE128_READY)) {
//        stop_all(FAIL_SPI_READING_128);
//    }
//    if (!transferData(CONFIG_SPI_SLAVE129_READY)) {
//        stop_all(FAIL_SPI_READING_129);
//    }
//    if (!transferData(CONFIG_SPI_SLAVE130_READY)) {
//        stop_all(FAIL_SPI_READING_130);
//    }
}

bool transferData(int encoder) {

    dbgOutputLoc(SPI_SENDING);

    GPIO_write(encoder, 0);

    bool transferOK = SPI_transfer(masterSpi, &transaction);

    GPIO_write(encoder, 1);
    return transferOK;
}

void *spiThread(void *arg0) {

    bool RDY = false;
    bool temp = false;

    while(1) {
        receiveFromEncoderQ(&temp);

        if (!RDY) {
            initEncoders();
            clearEncoderCounts();
            timer_setup();
//            sendMsgToReceiveQ(true, true, 1, 0, 0);
            RDY = true;
        }

//        dbgOutputLoc(SPI_READING_128);
        readEncoder(e128);

//        dbgOutputLoc(SPI_READING_129);
//        readEncoder(e129);
//
//        dbgOutputLoc(SPI_READING_130);
//        readEncoder(e130);

        clearEncoderCounts();
    }

}
