/*********************************************************** 
 *@file test_drive.h
 *@date 2017-10-03
 *@author Davide Asnaghi
***********************************************************/

#ifndef _TEST__DRIVE_H
#define _TEST__DRIVE_H	

#include "stm32f4xx_HAL.h"

#define RPM_MAX    100
#define RPM_MIN    0

// Drive function, takes RC joystic values as imput
void drive (int RX_X2 = 0, RX_Y1 = 0, RX_X1 = 0)

// Map range-A to range-B, used to set correct RPM
void float map(float x, float in_min, float in_max, float out_min, float out_max);