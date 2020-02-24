#include "debug.h"

UART_Handle uart = NULL;
UART_Params uartParams;
static QueueHandle_t uartQ = NULL;

void debug_setup() {

    GPIO_setConfig(CONFIG_GPIO_0, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_1, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_2, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_3, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_4, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_5, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_6, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_7, GPIO_CFG_OUTPUT);

    GPIO_write(CONFIG_GPIO_0, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_1, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_2, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_3, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_4, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_5, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_6, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_7, GPIO_CFG_OUT_LOW);

    UART_init();
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;

    uart = UART_open(CONFIG_UART_DEBUG, &uartParams);
    if (uart == NULL) {
        stop_all(FAILED_START_DEBUG);
    }

    uartQ = xQueueCreate(32, 1);
    if (uartQ == NULL) {
        stop_all(FAILED_START_DEBUG);
    }
}

void *uartThread(void *arg0) {

    while(1) {

        unsigned char outVal = receiveFromUARTQ();

        if ((int) outVal == -1) {
            stop_all(FAILED_START_DEBUG);
        }

        if (uart == NULL) {
            stop_all(FAILED_START_DEBUG);
        }

        const char e[1] = {outVal};
        UART_write(uart, e, sizeof(e));
    }
}

void dbgOutputLoc(unsigned int outLoc) {

    if (outLoc > 0x7B)
        stop_all(FAILED_UNKNOWN_CODE);

    GPIO_write(CONFIG_GPIO_0, GPIO_CFG_OUT_HIGH);

    GPIO_write(CONFIG_GPIO_1, outLoc & 0x01 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_2, outLoc & 0x02 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_3, outLoc & 0x04 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_4, outLoc & 0x08 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_5, outLoc & 0x10 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_6, outLoc & 0x20 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_7, outLoc & 0x40 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);

    GPIO_write(CONFIG_GPIO_0, GPIO_CFG_OUT_LOW);
}

void dbgUARTVal(unsigned char outVal) {

    xQueueSendToBackFromISR( uartQ, &outVal, 0);
}

unsigned char receiveFromUARTQ() {

    unsigned char msg = 0;

    xQueueReceive(uartQ, &msg, portMAX_DELAY);

    if (&msg == NULL) {
        return (unsigned char) -1;
    }

    return msg;
}

void stop_all(unsigned int FAILURE_CODE) {

    vTaskSuspendAll();
    taskENTER_CRITICAL();

    dbgOutputLoc(FAILURE_CODE);

    while(1) {

    }
}
