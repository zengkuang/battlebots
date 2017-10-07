
/*********************************************************** 
 *@file test_drive.c
 *@date 2017-10-06
 *@author Beck Pang
 *@architect Davide Asnaghi
 *
 
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

int16_t front_right;
int16_t back_right;
int16_t front_left;
int16_t back_left;

void drive_kinematics(int RX_X2, int RX_Y1, int RX_X1)
{
    //Remote Control Commands, Mapped to match min and max RPM
    int16_t drive  = (int16_t)map(RX_X2, RC_CH_VALUE_MIN, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);
    int16_t strafe = (int16_t)map(RX_Y1, RC_CH_VALUE_MIN, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);
    int16_t rotate = (int16_t)map(RX_X1, RC_CH_VALUE_MIN, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);

    // For later coordinate with Gimbal
    int rotate_feedback = 0;

    front_right = (-1*drive + strafe + rotate) + rotate_feedback;   // CAN ID: 0x201
    back_right = (drive + strafe + rotate) + rotate_feedback;       // CAN ID: 0x202
    front_left = (drive - strafe + rotate) + rotate_feedback;       // CAN ID: 0x203
    back_left = (-1*drive - strafe + rotate) + rotate_feedback;     // CAN ID: 0x204
    //Update using CAN bus chassis function (provided by Alex Wong)
    Chassis_Set_Speed(0.1*front_right, 0.1*back_right, 0.1*front_left, 0.1*back_left);
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
