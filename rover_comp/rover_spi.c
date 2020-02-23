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
static Timer_Handle timer0;

void spi_setup() {
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
    //spiParams.dataSize = 4;

    masterSpi = SPI_open(CONFIG_SPI_MASTER, &spiParams);
    if (masterSpi == NULL) {
        // Debug ERROR: Error initializing master SPI
    }
    else {
        // Debug: "Master SPI initialized\n"
    }

}

void timer_setup() {
    Timer_init();

    Timer_Params    params;

    /* Setting up the timer in continuous callback mode that calls the callback function */
    Timer_Params_init(&params);
    params.period = TIMER_LENGTH;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(CONFIG_TIMER_0, &params);

    if (timer0 == NULL) {
        /* Failed to initialized timer */
    }
    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
    }

    GPIO_write(CONFIG_SPI_MASTER_READY, 0);
}

void spi_close() {
    SPI_close(masterSpi);
    Timer_close(timer0);

    /* Example complete - set pins to a known state */
    GPIO_setConfig(CONFIG_SPI_SLAVE128_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_SPI_SLAVE129_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_SPI_SLAVE130_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_SPI_MASTER_READY, 0);
}

void timerCallback(Timer_Handle myHandle) {
    readEncoder(e128);
    readEncoder(e129);
    readEncoder(e130);
}

void readEncoder(int encoder) {

    if (encoder == e128) {
        GPIO_write(CONFIG_SPI_SLAVE128_READY, 0);
        transferData(encoder);
        GPIO_write(CONFIG_SPI_SLAVE128_READY, 1);
    }
    else if (encoder == e129) {
        GPIO_write(CONFIG_SPI_SLAVE129_READY, 0);
        transferData(encoder);
        GPIO_write(CONFIG_SPI_SLAVE129_READY, 1);
    }
    else if (encoder == e130) {
        GPIO_write(CONFIG_SPI_SLAVE130_READY, 0);
        transferData(encoder);
        GPIO_write(CONFIG_SPI_SLAVE130_READY, 1);
    }
}

void transferData(int encoder) {

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
    else {
        // Debug: "Unsuccessful master SPI transfer"
    }
}
