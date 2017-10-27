
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
int32_t front_right;
int32_t back_right;
int32_t front_left;
int32_t back_left;
float input_front_right =0;
float input_back_right =0;
float input_front_left=0;
float input_back_left=0;

PIDparameter FR_wheel={0,0,0.4,0.01};
PIDparameter FL_wheel={0,0,0.4,0.01};
PIDparameter BR_wheel={0,0,0.4,0.01};
PIDparameter BL_wheel={0,0,0.4,0.01};
extern volatile Encoder CM1Encoder; // CAN Address 201
extern volatile Encoder CM2Encoder; // CAN Address 202
extern volatile Encoder CM3Encoder; // CAN Address 203
extern volatile Encoder CM4Encoder; // CAN Address 204

void drive_kinematics(int RX_X2, int RX_Y1, int RX_X1)
{
    //Remote Control Commands, Mapped to match min and max RPM
    int32_t drive  = (int16_t)map(RX_X2, RC_CH_VALUE_MIN, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);
    int32_t strafe = (int16_t)map(RX_Y1, RC_CH_VALUE_MIN, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);
    int32_t rotate = (int16_t)map(RX_X1, RC_CH_VALUE_MIN, RC_CH_VALUE_MAX, RPM_MIN, RPM_MAX);

    // For later coordinate with Gimbal
    int rotate_feedback = 0;

    front_right = (-1*drive + strafe + rotate) + rotate_feedback;   // CAN ID: 0x201
    back_right = (drive + strafe + rotate) + rotate_feedback;       // CAN ID: 0x202
    front_left = (drive - strafe + rotate) + rotate_feedback;       // CAN ID: 0x203
    back_left = (-1*drive - strafe + rotate) + rotate_feedback;     // CAN ID: 0x204
    //Update using CAN bus chassis function (provided by Alex Wong)
		input_front_right = speedPID(front_right/8.0,CM1Encoder,&FR_wheel);
		input_back_right = speedPID(back_right/8.0,CM2Encoder,&BR_wheel);
		input_back_left = speedPID(back_left/8.0,CM4Encoder,&BL_wheel);
		input_front_left = speedPID(front_left/8.0,CM3Encoder,&FL_wheel);
		
		Chassis_Set_Speed(input_front_right, input_back_right, input_front_left, input_back_left);
	
	
    //Chassis_Set_Speed(0.1*front_right, 0.1*back_right, 0.1*front_left, 0.1*back_left);
}


float speedPID(float target_value,Encoder encoder,PIDparameter* s){
				float output;
				s->error = target_value - encoder.velocity_from_ESC;
				s->error_sum += s->error;
	
	
				if(s->error_sum > 100000){
						s->error_sum = 100000;
				}
				if(s->error_sum < -100000)
				{	
						s->error_sum = -100000;
				}
				
				output = s->Kp*s->error + s->Ki*s->error_sum;
				return output; 
}




float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
