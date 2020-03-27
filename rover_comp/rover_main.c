/*
 * star.c
 *
 *  Created on: Feb 5, 2020
 *      Author: giffen
 */

#include <rover_main.h>
#include <rover_uart.h>
#include <math.h>

#define conversionValue_128     1.6429
#define conversionValue_129     1.6669

char convertEncoderToMotor_128(int encoderValue) {

    double ret = encoderValue;

    return ceil(ret/conversionValue_128);

}

char findNewSpeed(char *curSpeed, char *outputSpeed) {
    return *curSpeed + *outputSpeed;
}

char convertEncoderToMotor_129(int encoderValue) {

    double ret = encoderValue;

    return ceil(ret/conversionValue_129);

}

void PIDalg (struct PIDvalues *motor, char measuredValue) {

    long error = motor->desiredSpeed - measuredValue;
    motor->integral += error;
    long outputVal = KP*error + KI*motor->integral;

    motor->currentSpeed += outputVal;

}

/*
 *  @function   mainThread
 *              Main thread that will pull from the sensor queue to
 *              change the FSM states.
 *
 *  @params     arg0
 *  @return     void
 */
void *mainThread(void *arg0)
{

    char currentSpeed_128 = 16;
    //char currentSpeed_129 = convertEncoderToMotor_129(desiredSpeed);
    //char currentSpeed_128 = convertEncoderToMotor_130(desiredSpeed);

    /**********************************/
    dbgOutputLoc(STAR_MAIN_START); 
    /**********************************/

    struct receiveData curData = {false, 0, 0};

    // sensor FSM
    //struct fsmData fsm = {Init, 0, 0, 0, 0};

    char desiredSpeed = 0xFF; // 25%

    struct PIDvalues PID128 = {desiredSpeed,0,0,0};

    while(1) {
        /**********************************/
        dbgOutputLoc(STAR_WHILE_BEGIN); 
        /**********************************/

        // wait for message (blocking)
        /**********************************/
        dbgOutputLoc(STAR_WAIT_MESSAGE); 
        /**********************************/
        receiveFromMQTTReceiveQ(&curData);

        if (curData.sensorType == false) {
            dbgOutputLoc(curData.data2);


            /*
            *      with each new command, reset the integral, outputVal, currentSpeed
            *
            *      measured_value = curData.data;
            *      error = desiredSpeed - measured_value;
            *      integral += error;
            *      outputVal = Kp*error + Ki*integral;
            *
            *      currentSpeed += convertEncoderToMotor_XXX(outputVal);
            *
            *  Setting the command to 0 will DECREMENT THE COUNTER from 0xFFFFFFFF
            *  Setting the command to 1 will INCREMENT THE COUNTER from 0x00000000
            */



            int i;
            for (i=0;i<sizeof(curData.data);i++) {
                dbgOutputLoc((curData.data >> (sizeof(curData.data)-1-i)*8) & 0xFF);
            }
        }

        sendMsgToMotorsQ(128, 1, desiredSpeed);
        //sendMsgToMotorsQ(129, 1, 31);
        //sendMsgToMotorsQ(130, 1, desiredSpeed);

        dbgOutputLoc(desiredSpeed);

        /**********************************/
        dbgOutputLoc(STAR_RECEIVE_MESSAGE);
        /**********************************/

    }
}
