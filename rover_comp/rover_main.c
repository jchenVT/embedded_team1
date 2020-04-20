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
#define desiredMovementTicks    0x2F
#define stopTicks               0

char convertTicksToMotor_128(long ticks) {

    double ret = ticks/conversionValue_128;

    if (ret < 0) {
        return 0;
    }
    else if (ret > 127) {
        return 127;
    }

    return floor(ret);

}

char findNewSpeed(char *curSpeed, char *outputSpeed) {
    return *curSpeed + *outputSpeed;
}

char convertTicksToMotor_129(long ticks) {

    double ret = ticks/conversionValue_129;

    if (ret < 0) {
        return 0;
    }
    else if (ret > 127) {
        return 127;
    }

    return floor(ret);

}

void PIDalg (struct PIDvalues *motor, long measuredValue) {

    long error = 0;

    if (motor->direction == 0) {
        error = motor->desiredTicks - (0xFFFFFFFF - measuredValue);
    }
    else {
        error = motor->desiredTicks - measuredValue;
    }
    motor->integral += error;
    motor->currentTicks += floor(KP*error + KI*motor->integral);

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
    /**********************************/
    dbgOutputLoc(STAR_MAIN_START); 
    /**********************************/

    struct receiveData curData = {false, false, 0, 0, 0};

    // sensor FSM
    //struct fsmData fsm = {Init, 0, 0, 0, 0};

    struct PIDvalues PID128 = {desiredMovementTicks,desiredMovementTicks,0,0,1};
    struct PIDvalues PID129 = {desiredMovementTicks,desiredMovementTicks,0,0,1};
    struct PIDvalues PID130 = {desiredMovementTicks,0,0,0,1};

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

            long ticks = (long)curData.data;

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
            if (curData.data2 == 128) {
                PIDalg(&PID128, ticks);
            }

            int i;
            for (i=0;i<sizeof(ticks);i++) {
                dbgOutputLoc((ticks >> (sizeof(ticks)-1-i)*8) & 0xFF);
            }
        }
        else {
            /*
             * Change PIDValues:
             *      integral = 0
             *      error = 0
             *      direction set to new instruction
             *      desired speed = 0x2F or 0
             */


        }

        sendMsgToMotorsQ(128, PID128.direction, convertTicksToMotor_128(PID128.currentTicks));
        sendMsgToMotorsQ(129, PID129.direction, convertTicksToMotor_128(PID128.currentTicks));
        sendMsgToMotorsQ(130, PID130.direction, convertTicksToMotor_128(PID128.currentTicks));

        /**********************************/
        dbgOutputLoc(STAR_RECEIVE_MESSAGE);
        /**********************************/

    }
}
