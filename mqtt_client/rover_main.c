/*
 * star.c
 *
 *  Created on: Feb 5, 2020
 *      Author: giffen
 */

#include <rover_main.h>
#include <rover_uart.h>
#include <math.h>
#include <mqtt_queue.h>
#include <jsonFormat.h>
#include <rover_spi.h>

/* Timer */
#include <ti/drivers/Timer.h>
#include <FreeRTOS.h>
#include <timers.h>

/*
 * roverRadius = 71mm
 * roverCirc = 446.106mm
 * 1degree = 2.4784mm
 */


#define conversionValue_128             0.3229
//#define conversionValue_129             1.6669
#define conversionTicktoRotationValue   1
#define conversionTicktoForwardValue    1
#define desiredMovementTicks            0x30
#define stopTicks                       0

static double KP = 0;
static double KI = 0;

static int attemptPubCount;
static int recvSubCount;
static int counter;
static bool status;
static enum roverStates state;

char convertTicksToMotor_128(long ticks) {

    double ret = (double)ticks*conversionValue_128;

    if (ret <= 0) {
        return 0;
    }
    else if (ret >= 127) {
        return 127;
    }

    return (char)floor(ret);

}

//char convertTicksToMotor_129(long ticks) {
//
//    double ret = ticks/conversionValue_129;
//
//    if (ret < 0) {
//        return 0;
//    }
//    else if (ret > 127) {
//        return 127;
//    }
//
//    return (char)floor(ret);
//
//}

//char convertTicksToRotDist(long ticks) {
//    double ret = ticks/conversionValue_129;
//
//    if (ret < 0) {
//        return 0;
//    }
//    else if (ret > 127) {
//        return 127;
//    }
//
//    return floor(ret);
//}

//char convertTicksToForwDist(long ticks) {
//
//}

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
void PIDalg (struct PIDvalues *motor, long measuredValue) {
    long error = 0;

    if (motor->direction == 0) {
        error = motor->desiredTicks - (0xFFFFFFFF - measuredValue);
        packageEncoderJSON(0, convertTicksToMotor_128(0xFFFFFFFF - measuredValue));
    }
    else {
        error = motor->desiredTicks - measuredValue;
        packageEncoderJSON(1, convertTicksToMotor_128(measuredValue));
    }

    motor->integral += error;
    motor->currentTicks += (long)floor(KP*error + KI*motor->integral);

}

void stopWheel(struct PIDvalues *wheel) {
    wheel->currentTicks = stopTicks;
    wheel->desiredTicks = stopTicks;
    wheel->integral = 0;
    wheel->prevError = 0;
    wheel->direction = 1;
}

void setToLeftTurn(struct PIDvalues *wheel) {
    wheel->currentTicks = desiredMovementTicks;
    wheel->desiredTicks = desiredMovementTicks;
    wheel->integral = 0;
    wheel->prevError = 0;
    wheel->direction = 0;
}

void setToRightTurn(struct PIDvalues *wheel) {
    wheel->currentTicks = desiredMovementTicks;
    wheel->desiredTicks = desiredMovementTicks;
    wheel->integral = 0;
    wheel->prevError = 0;
    wheel->direction = 1;
}

void updateState(enum roverStates prevState, enum roverStates curState,
                 struct PIDvalues *PID128, struct PIDvalues *PID129,
                 struct PIDvalues *PID130) {

    if (prevState != curState) {
        switch(curState) {
            case stop:
                stopWheel(PID128);
                stopWheel(PID129);
                stopWheel(PID130);
                break;
            case turn_left:
                setToLeftTurn(PID128);
                setToLeftTurn(PID129);
                setToLeftTurn(PID130);
                break;
            case turn_right:
                setToRightTurn(PID128);
                setToRightTurn(PID129);
                setToRightTurn(PID130);
                break;
            case move_forward:
                setToLeftTurn(PID128);
                setToRightTurn(PID129);
                PID129->currentTicks += 6;
                stopWheel(PID130);
                break;
            case target:
                stopWheel(PID128);
                stopWheel(PID129);
                stopWheel(PID130);
                break;
        }
    }

    sendMsgToMotorsQ(128, PID128->direction, convertTicksToMotor_128(PID128->currentTicks));
    sendMsgToMotorsQ(129, PID129->direction, convertTicksToMotor_128(PID129->currentTicks));
    sendMsgToMotorsQ(130, PID130->direction, convertTicksToMotor_128(PID130->currentTicks));
}

/*
 *  @function   mainThread
 *              Main thread that will pull from the sensor queue to
 *              change the FSM states.
 *
 *  @params     arg0
 *  @return     void
 */
void *mainRoverThread(void *arg0)
{
    /**********************************/
    dbgOutputLoc(STAR_MAIN_START); 
    /**********************************/

    struct receiveData curData = {false, false, 0, 0, 0};

    attemptPubCount = 0;
    recvSubCount = 0;
    counter = 0;
    status = true; // working
    bool spi_OFF = true;
    TimerHandle_t timerDebug = xTimerCreate("DebugTimer", pdMS_TO_TICKS(1000), pdTRUE, NULL, timerCallbackDebug);
    xTimerStart(timerDebug, 0);

    state = stop;
    struct PIDvalues PID128 = {stopTicks,stopTicks,0,0,1};
    struct PIDvalues PID129 = {stopTicks,stopTicks,0,0,1};
    struct PIDvalues PID130 = {stopTicks,stopTicks,0,0,1};

    while(1) {
        /**********************************/
        dbgOutputLoc(STAR_WHILE_BEGIN);
        /**********************************/
        receiveFromMQTTReceiveQ(&curData);

        if (spi_OFF) {

            KP = curData.data;
            KI = curData.data2;

            packageStartingJSON();

            sendMsgToEncoderQ();
            spi_OFF = false;
        }

        if (curData.sensorType == false) {
            dbgOutputLoc(curData.data2);

            long ticks = (long)curData.data;

            if (curData.data2 == 128) {
                PIDalg(&PID128, ticks);
            }

            updateState(state, state, &PID128, &PID129, &PID130);
        }
        else {
            recvSubCount++;
            if (curData.point_move) {
                if (curData.angle_rotate == 0) {
                    dbgOutputLoc(0xF1);
                    updateState(state, move_forward, &PID128, &PID129, &PID130);
                    state = move_forward;
                }
                else if (curData.angle_rotate < 0) {
                    dbgOutputLoc(0xF2);
                    updateState(state, turn_left, &PID128, &PID129, &PID130);
                    state = turn_left;
                }
                else {
                    dbgOutputLoc(0xF3);
                    updateState(state, turn_right, &PID128, &PID129, &PID130);
                    state = turn_right;
                }
            }
            else {
                dbgOutputLoc(0xF4);
                updateState(state, stop, &PID128, &PID129, &PID130);
                state = stop;
            }

            if (packageRoverJSON(state) == 1) {
                attemptPubCount++;
            }
            else {
                status = false;
            }
        }

        /**********************************/
        dbgOutputLoc(STAR_RECEIVE_MESSAGE);
        /**********************************/
    }
}

void timerCallbackDebug(TimerHandle_t xTimer) {

    // publish to message queue
    if (packageDebugJSON(attemptPubCount, recvSubCount, status, "rover", "arm_sensor") == 1) {
        status = true;
    }
    else {
        // UART_PRINT("[ERROR]: Publish request not sent... \n\r");
    }
}
