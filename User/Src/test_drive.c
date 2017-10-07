
/*********************************************************** 
 *@file test_drive.c
 *@date 2017-10-03
 *@author Davide Asnaghi
 
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
#include "ChassisMotors.h"
#include "ChassisMotors.c"

void drive (int RX_X2 = 0, RX_Y1 = 0, RX_X1 = 0)
{	 

	//Set deadzone to 6% range to provide smoother control
	THRESHOLD = (RC_CH_VALUE_MAX - RC_CH_VALUE_MIN)*3/100;

	//Create "deadzone" for Y1
	if(abs(RX_Y1) < THRESHOLD)
		RX_Y1 = 0;	

	//Create "deadzone" for X1
	if(abs(RX_X1) < THRESHOLD)
		RX_X1 = 0;

	//Create "deadzone" for X2
	if(abs(RX_X2) < THRESHOLD)
		RX_X2 =	0;

	//Remote Control Commands, Mapped to match min and max RPM
	float front_right = map ((Y1 - X2 - X1), RC_CH_VALUE_MAX, RC_CH_VALUE_MAX, RMP_MIN, RPM_MAX)
	float back_right = map ((Y1 - X2 + X1), RC_CH_VALUE_MAX, RC_CH_VALUE_MAX, RMP_MIN, RPM_MAX)
	float front_left = map ((Y1 + X2 + X1), RC_CH_VALUE_MAX, RC_CH_VALUE_MAX, RMP_MIN, RPM_MAX)
	float back_left = map ((Y1 + X2 - X1), RC_CH_VALUE_MAX, RC_CH_VALUE_MAX, RMP_MIN, RPM_MAX)

	//Update using CAN bus chassis function (provided by Alex Wong)
	CHASSIS_UPDATE(front_right, back_right, front_left, back_left)
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
