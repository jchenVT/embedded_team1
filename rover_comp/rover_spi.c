/*
 * timerone.c
 *
 *  Created on: Feb 20, 2020
 *      Author: trieu
 */

#include <rover_spi.h>

#define TIMER_LENGTH 1000

unsigned char masterRxBuffer[SPI_MSG_LENGTH];
unsigned char masterTxBuffer[SPI_MSG_LENGTH];

/* Semaphore to block master until slave is ready for transfer */
static sem_t masterSem;
static SPI_Handle masterSpi;

/*
 *  ======== slaveReadyFxn ========
 *  Callback function for the GPIO interrupt on CONFIG_SPI_SLAVE_READY.
 */
void slaveReadyFxn(uint_least8_t index) {
    sem_post(&masterSem);
}

void spi_setup() {
    /* Call driver init functions. */
    GPIO_init();
    SPI_init();
}

void spi_close() {
    SPI_close(masterSpi);

    /* Example complete - set pins to a known state */
    GPIO_disableInt(CONFIG_SPI_SLAVE_READY);
    GPIO_setConfig(CONFIG_SPI_SLAVE_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_SPI_MASTER_READY, 0);
}

void *mainSPIThread(void *arg0) {
    SPI_Params      spiParams;
    SPI_Transaction transaction;
    uint32_t        i;
    bool            transferOK;
    int32_t         status;

    /*
     * Handshake - Set CONFIG_SPI_MASTER_READY high to indicate master is ready
     * to run.  Wait CONFIG_SPI_SLAVE_READY to be high.
     */
    GPIO_write(CONFIG_SPI_MASTER_READY, 1);
    while (GPIO_read(CONFIG_SPI_SLAVE_READY) == 0) {}

    /* Handshake complete; now configure interrupt on CONFIG_SPI_SLAVE_READY */
   GPIO_setConfig(CONFIG_SPI_SLAVE_READY, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
   GPIO_setCallback(CONFIG_SPI_SLAVE_READY, slaveReadyFxn);
   GPIO_enableInt(CONFIG_SPI_SLAVE_READY);

   /*
    * Create synchronization semaphore; the master will wait on this semaphore
    * until the slave is ready.
    */
   status = sem_init(&masterSem, 0, 0);
   if (status != 0) {
       /* DEBUG ERROR: Error creating masterSem */
       while(1);
   }

   /* Open SPI as master (default) */
   SPI_Params_init(&spiParams);
   spiParams.frameFormat = SPI_POL0_PHA1;
   spiParams.bitRate = 10000000;
   masterSpi = SPI_open(CONFIG_SPI_MASTER, &spiParams);
   if (masterSpi == NULL) {
       // Debug ERROR: Error initializing master SPI
       while (1);
   }
   else {
       // Debug: "Master SPI initialized\n"
   }

   /*
    * Master has opened CONFIG_SPI_MASTER; set CONFIG_SPI_MASTER_READY high to
    * inform the slave.
    */
   GPIO_write(CONFIG_SPI_MASTER_READY, 0);

   /* Copy message to transmit buffer */
   strncpy((char *) masterTxBuffer, MASTER_MSG, SPI_MSG_LENGTH);

   while (1) {
       /*
        * Wait until slave is ready for transfer; slave will pull
        * CONFIG_SPI_SLAVE_READY low.
        */
       sem_wait(&masterSem);

       /* Initialize master SPI transaction structure */
       masterTxBuffer[sizeof(MASTER_MSG) - 1] = (i % 10) + '0';
       memset((void *) masterRxBuffer, 0, SPI_MSG_LENGTH);
       transaction.count = SPI_MSG_LENGTH;
       transaction.txBuf = (void *) masterTxBuffer;
       transaction.rxBuf = (void *) masterRxBuffer;

       /* Toggle user LED, indicating a SPI transfer is in progress */
       //GPIO_toggle(CONFIG_GPIO_LED_1);

       /* Perform SPI transfer */
       transferOK = SPI_transfer(masterSpi, &transaction);
       if (transferOK) {
           // Debug:"Master received: %s", masterRxBuffer
       }
       else {
           // DEBUG ERROR: "Unsuccessful master SPI transfer"
       }
   }

}
