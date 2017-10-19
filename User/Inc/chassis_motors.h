/**
  *@file test_can.h
  *@date 2016-12-12
  *@author Albert.D
  *@brief 
  */
  
#ifndef _TEST__CAN_H
#define _TEST__CAN_H

#include "stm32f4xx_HAL.h"

#define ENCODER_MAX_RANGE 8172
#define ENCODER_ANGLE     360.0f  // Use 360 degree as a round

#define ENCODER_RATE_BUF_SIZE 3
typedef struct {
    int16_t velocity_from_ESC;
    int16_t position_raw_value;
    int16_t position_raw_value_last;
    int32_t position_ecd_value;
    int32_t position_diff;
    int32_t count;
    uint8_t buf_count;  // Filter buffer count
    int32_t position_ecd_bias; // Initial encoder position
    int32_t velocity_ecd_raw;  // velocity from encoder difference
    int32_t velocity_ecd_raw_last;
    int32_t velocity_buff[ENCODER_RATE_BUF_SIZE];
    int32_t round_count;
    int32_t velocity_filtered;
    float ecd_angle;
}Encoder;

//typedef struct {
//    Encoder ecd;
//    int32_t motor_current_set_point;
//}Chassis_Motor_Type;

extern uint8_t can1_rx_data[8];
extern uint8_t can2_rx_data[8];

void CanFilter_Init(CAN_HandleTypeDef* hcan);
void encoderProcess(volatile Encoder*, CanRxMsgTypeDef*);
void Chassis_Set_Speed(int16_t, int16_t, int16_t, int16_t);
void CAN_Send_Msg(CAN_HandleTypeDef* hcan, uint8_t *msg, uint32_t id);

#endif

