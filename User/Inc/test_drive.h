/*********************************************************** 
 *@file test_drive.h
 *@date 2017-10-07
 *@author Beck Pang
 *@architect Davide Asnaghi
***********************************************************/

#ifndef __TEST__DRIVE_H
#define __TEST__DRIVE_H	

#include "stm32f4xx_HAL.h"

#define RPM_MAX_820R    ((int16_t) 4000)
#define RPM_MIN_820R    ((int16_t)-4000)

// Drive function, takes RC joystic values as imput
void drive_kinematics(int, int, int);

// Map range-A to range-B, used to set correct RPM
float map(float x, float in_min, float in_max, float out_min, float out_max);
#endif /* __TEST__DRIVE_H */
