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

    Timer_init();
    ADC_init();
    Timer_Params_init(&params);

    params.periodUnits = Timer_PERIOD_US;
    params.period = TIMER_PERIOD;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timer75Callback;

    timer2 = Timer_open(CONFIG_TIMER_0, &params);

    if (timer2 == NULL) {
        /* Failed to initialized timer */
        while (1) {}
    }

    if (Timer_start(timer2) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
        while (1) {}
    }
    return (NULL);
}

int convertToMM(uint32_t mV) {
    int voltage = (int) mV / VCONVERSION;
    int mm = pow(MMCONVERSION1 * voltage, (MMCONVERSION2));
    return mm;
}

void timer75Callback() {
    ADC_Handle adc;
    ADC_Params params;
    int_fast16_t res = 0;
    uint16_t adcValue;
    uint32_t adcValueUv;
    int i;

    ADC_Params_init(&params);
    adc = ADC_open(CONFIG_ADC_0, &params);

    if (adc == NULL) {
        while (1) {}
    }

    /* sampling 10 times */
    for (i = 0; i < 10; ++i) {
        res += ADC_convert(adc, &adcValue);
    }

    res /= 10;

    if (res == ADC_STATUS_SUCCESS) {
        adcValueUv = ADC_convertToMicroVolts(adc, adcValue);
    }

    /* Converting to millimeters */

    sendSensorMsgToQ1(convertToMM(adcValueUv));

    ADC_close(adc);
}
