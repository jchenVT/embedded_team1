/*
 * rover_debug.c
 *
 *  Created on: Feb 20, 2020
 *      Author: trieu
 */

#include <rover_debug.h>

void debug_setup()
{
    GPIO_init();

    GPIO_setConfig(CONFIG_GPIO_TOGGLE, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_0, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_1, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_2, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_3, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_4, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_5, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_6, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_7, GPIO_CFG_OUTPUT);

    GPIO_write(CONFIG_GPIO_TOGGLE, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_0, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_1, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_2, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_3, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_4, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_5, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_6, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_7, GPIO_CFG_OUT_LOW);
}

void dbgOutputLoc(char outLoc)
{
    if (outLoc > 0x93) {
        //stop_all(FAIL_UNKNOWN_CODE);
    }

    GPIO_write(CONFIG_GPIO_TOGGLE, GPIO_CFG_OUT_HIGH);

    GPIO_write(CONFIG_GPIO_0, outLoc & 0x01 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_1, outLoc & 0x02 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_2, outLoc & 0x04 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_3, outLoc & 0x08 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_4, outLoc & 0x10 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_5, outLoc & 0x20 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_6, outLoc & 0x40 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_7, outLoc & 0x80 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);

    // state change
    GPIO_write(CONFIG_GPIO_TOGGLE, GPIO_CFG_OUT_LOW);
}

void stop_all(unsigned int FAILURE_CODE)
{
    vTaskSuspendAll();
    taskENTER_CRITICAL();

    uart_close();
    spi_close();

    /*****************************/
    dbgOutputLoc(FAILURE_CODE);
    /*****************************/

    while(1)
    {
    }

    taskEXIT_CRITICAL();
}
