/*
 * timertwo.c
 *
 *  Created on: Feb 4, 2020
 *      Author: christina
 */


#include "timertwo.h"

void *mainTimerTwoThread(void *arg0) {
    Timer_Handle timer2;
    Timer_Params params;

    /**************************/
    dbgOutputLoc(T2_TIMER_INITIALIZE);
    /**************************/

    Timer_Params_init(&params);

    params.periodUnits = Timer_PERIOD_US;
    params.period = TIMER_PERIOD;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timer75Callback;

    timer2 = Timer_open(CONFIG_TIMER_1, &params);

    /**************************/
    dbgOutputLoc(T2_TIMER_OPENED);
    /**************************/

    if (timer2 == NULL) {
        /* Failed to initialized timer */
        stop_all(FAIL_T2_INIT);
    }

    if (Timer_start(timer2) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
        stop_all(FAIL_T2_START);
    }
    return (NULL);
}

int convertToMM(uint32_t mV) {
    int voltage = (int) mV / VCONVERSION;
    // 20 = 4 mV
    // 30 = 6
    int mm = pow(MMCONVERSION1 * voltage, (MMCONVERSION2));
    return mm;
}

void timer75Callback(Timer_Handle myHandle) {
    ADC_Handle adc;
    ADC_Params params;
    int_fast16_t res = 0;
    uint16_t avgValue = 0;
    uint16_t adcValue;
    int i;


    /**************************/
    dbgOutputLoc(T2_CALLBACK_BEGIN);
    /**************************/

    ADC_Params_init(&params);
    adc = ADC_open(CONFIG_ADC_0, &params);

    if (adc == NULL) {
        stop_all(FAIL_ADC_INIT);
    }

    /* sampling 10 times */
    for (i = 0; i < 10; ++i) {
        res = ADC_convert(adc, &adcValue);
        avgValue += adcValue;
    }

    /**************************/
    dbgOutputLoc(T2_CALLBACK_ADC_READ);
    /**************************/

    avgValue /= 10;
    // uart will print but it will not print consecutively.
    // sensor values are usually 3-5.

    dbgUARTVal('{');
    if (res == ADC_STATUS_SUCCESS) {
        // send to UART
        char UARTbuf[10];
        int retnum;
        retnum = snprintf(UARTbuf, 10, "%d", avgValue);
        for(i=0;i<retnum;i++)
        {
            dbgUARTVal(UARTbuf[i]);
        }
    }
    dbgUARTVal('}');
    /* Converting to millimeters */

    if (sendSensorMsgToQ1(convertToMM(avgValue)) == errQUEUE_FULL) {
        stop_all(FAIL_T2_SEND_TO_Q1);
    }

    ADC_close(adc);
}
