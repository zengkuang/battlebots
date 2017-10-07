
/*********************************************************** 
 *@file test_drive.c
 *@date 2017-10-06
 *@author Davide Asnaghi
 * @review Beck Pang
 
 The function provides a quick way to drive the motor from 
 the Remote Control input, using the following variables

 RX_X1	LEFT X axis 	Joystic
 RX_Y1	LEFT Y axis 	Joystic
 RX_X2 	RIGHT X axis	Joystic

 The following are standard library definitions for RC

 RC_CH_VALUE_MAX	Max value from Joystic
 RC_CH_VALUE_Min	Min value from Joystic
************************************************************/



#include "test_drive.h"
#include "test_DBUS.h"
#include "chassis_motors.h"

void drive(int RX_X2, int RX_Y1, int RX_X1)
{
    /*
    //Set deadzone to 6% range to provide smoother control
    float THRESHOLD = (RC_CH_VALUE_MAX - RC_CH_VALUE_MIN)*3/100;

    //Create "deadzone" for Y1
    if(abs(RX_Y1) > THRESHOLD)
        RX_Y1 = 0;

    //Create "deadzone" for X1
    if(abs(RX_X1) > THRESHOLD)
        RX_X1 = 0;

    //Create "deadzone" for X2
    if(abs(RX_X2) > THRESHOLD)
        RX_X2 =	0;
     */

    //Remote Control Commands, Mapped to match min and max RPM
    int16_t front_right   = (int16_t)map((RX_Y1 - RX_X2 - RX_X1), RC_CH_VALUE_MAX, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);
    int16_t back_right    = (int16_t)map((RX_Y1 - RX_X2 + RX_X1), RC_CH_VALUE_MAX, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);
    int16_t front_left    = (int16_t)map((RX_Y1 + RX_X2 + RX_X1), RC_CH_VALUE_MAX, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);
    int16_t back_left     = (int16_t)map((RX_Y1 + RX_X2 - RX_X1), RC_CH_VALUE_MAX, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);

    //Update using CAN bus chassis function (provided by Alex Wong)
    Chassis_Set_Speed(front_right, back_right, front_left, back_left);
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
