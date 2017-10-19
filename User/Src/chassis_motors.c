/**
  *@file chassis_motors.c
  *@date 2017-10-19
  *@author Beck Pang
  *@brief 
  */

#include <chassis_motors.h>
#include <stm32f4xx_hal_can.h>
#include "can.h"
#include "arm_math.h"
#include "chassis_motors.h"


uint8_t can1_rx_data[8];
uint8_t can2_rx_data[8];

// Chassis Motor Encoder
volatile Encoder CM1Encoder = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // CAN Address 201
volatile Encoder CM2Encoder = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // CAN Address 202
volatile Encoder CM3Encoder = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // CAN Address 203
volatile Encoder CM4Encoder = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // CAN Address 204
volatile Encoder CM5Encoder = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // CAN Address 205 pitch
volatile Encoder CM6Encoder = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // CAN Address 206 yaw

uint32_t can_chassis_count[4] = {0, 0, 0, 0};

extern arm_pid_instance_f32 CMotorPID;

float32_t CM1velocity;
float32_t CM2velocity;
float32_t CM3velocity;
float32_t CM4velocity;

/*
 * can filter must be initialized before use
 */
void CanFilter_Init(CAN_HandleTypeDef *hcan)
{
    CAN_FilterConfTypeDef canfilter;

    //create memory to save the message, if not will raise error
    static CanTxMsgTypeDef Tx1Message;
    static CanRxMsgTypeDef Rx1Message;
    static CanTxMsgTypeDef Tx2Message;
    static CanRxMsgTypeDef Rx2Message;

    canfilter.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilter.FilterScale = CAN_FILTERSCALE_32BIT;

    //filtrate any ID you want here
    canfilter.FilterIdHigh = 0x0000;
    canfilter.FilterIdLow = 0x0000;
    canfilter.FilterMaskIdHigh = 0x0000;
    canfilter.FilterMaskIdLow = 0x0000;

    canfilter.FilterFIFOAssignment = CAN_FilterFIFO0;
    canfilter.FilterActivation = ENABLE;
    canfilter.BankNumber = 14;

    //use different filter for can1&can2
    if (hcan == &hcan1) {
        canfilter.FilterNumber = 0;
        hcan->pTxMsg = &Tx1Message;
        hcan->pRxMsg = &Rx1Message;
    }
    if (hcan == &hcan2) {
        canfilter.FilterNumber = 14;
        hcan->pTxMsg = &Tx2Message;
        hcan->pRxMsg = &Rx2Message;
    }

    HAL_CAN_ConfigFilter(hcan, &canfilter);

}

/*
 * Overload the interrupt function beneath
 * it will be auto callback when can receive msg completely
 */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan == &hcan1) {       // CAN bus 1 is for drive motors
        if (hcan->pRxMsg->IDE == CAN_ID_STD) {  // Check correct CAN Identifier Type

            switch (hcan->pRxMsg->StdId) {

                case 0x201:     // Motor ID = 1
                    encoderProcess(&CM1Encoder, hcan->pRxMsg);
                    break;
                case 0x202:     // Motor ID = 2
                    encoderProcess(&CM2Encoder, hcan->pRxMsg);
                    break;
                case 0x203:     // Motor ID = 3
                    encoderProcess(&CM3Encoder, hcan->pRxMsg);
                    break;
                case 0x204:     // Motor ID = 4
                    encoderProcess(&CM4Encoder, hcan->pRxMsg);
                    break;
                case 0x205:
                    encoderProcess(&CM5Encoder, hcan->pRxMsg);
                    break;
                case 0x206:
                    encoderProcess(&CM6Encoder, hcan->pRxMsg);
                    break;
            }

        }
        __HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0);
    }

    HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);
    HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0);
}

/*
 *  Input   can message
 *  Process the can bus message
 * */
void encoderProcess(volatile Encoder *ecd, CanRxMsgTypeDef *msg)
{
    // Initialize the angle
    ecd->position_raw_value_last = ecd->position_raw_value;

    // Bitwise OR operation to extract the 16-bit information
    ecd->velocity_from_ESC = (msg->Data[2] << 8) | msg->Data[3];
    ecd->position_raw_value = (msg->Data[0] << 8) | msg->Data[1];

    ecd->position_diff = ecd->position_raw_value - ecd->position_raw_value_last;

    // If the feedback from the encoder changes too much, add the rotation
    if (ecd->position_diff < -7500) {
        ecd->round_count--;
        ecd->velocity_ecd_raw = ecd->position_diff + ENCODER_MAX_RANGE;
    } else if (ecd->position_diff > 7500) {
        ecd->round_count++;
        ecd->velocity_ecd_raw = ecd->position_diff - ENCODER_MAX_RANGE;
    } else {
        ecd->velocity_ecd_raw = ecd->position_diff;
    }

    // Calculate the encoder output in a continuous value domain
    ecd->position_ecd_value = ecd->position_raw_value + ecd->round_count * ENCODER_MAX_RANGE;

    // Convert the continuous encoder to continuous angle
    ecd->ecd_angle = (float) (ecd->position_raw_value - ecd->position_ecd_bias) * ENCODER_ANGLE / ENCODER_MAX_RANGE \
            + ecd->round_count * ENCODER_ANGLE;

    ecd->velocity_buff[ecd->buf_count++] = ecd->velocity_ecd_raw;

    if (ecd->buf_count == ENCODER_RATE_BUF_SIZE) {ecd->buf_count = 0;}

    // Calculate the velocity average value
    int8_t i = 0;
    int32_t temp_sum = 0;
    for (i = 0; i < ENCODER_RATE_BUF_SIZE; i++) {
        temp_sum += ecd->velocity_buff[i];
    }
    ecd->velocity_filtered = (int32_t) (temp_sum / ENCODER_RATE_BUF_SIZE);
}

/*
 * PID Closed loop speed controller for chassis motors
 * Input  target speed
 * Output CAN bus command for the motor current
 */
void Chassis_Set_Speed(int16_t RPM1, int16_t RPM2, int16_t RPM3, int16_t RPM4)
{
    CM1velocity = arm_pid_f32(&CMotorPID, (float32_t)(RPM1 - CM1Encoder.velocity_from_ESC)) + RPM1;
    CM2velocity = arm_pid_f32(&CMotorPID, (float32_t)(RPM2 - CM2Encoder.velocity_from_ESC)) + RPM2;
    CM3velocity = arm_pid_f32(&CMotorPID, (float32_t)(RPM3 - CM3Encoder.velocity_from_ESC)) + RPM3;
    CM4velocity = arm_pid_f32(&CMotorPID, (float32_t)(RPM4 - CM4Encoder.velocity_from_ESC)) + RPM4;

    uint8_t can1_message_chassis[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    can1_message_chassis[0] = RPM1 >> 8;
    can1_message_chassis[1] = RPM1;
    can1_message_chassis[2] = RPM2 >> 8;
    can1_message_chassis[3] = RPM2;
    can1_message_chassis[4] = RPM3 >> 8;
    can1_message_chassis[5] = RPM3;
    can1_message_chassis[6] = RPM4 >> 8;
    can1_message_chassis[7] = RPM4;

    CAN_Send_Msg(&hcan1, can1_message_chassis, 0x200);
}

/*
 * CAN Send Message function for DJI components only
 */
void CAN_Send_Msg(CAN_HandleTypeDef *hcan, uint8_t *msg, uint32_t id)
{
    uint8_t index = 0;
    for (index = 0; index < 8; index++) { hcan->pTxMsg->Data[index] = msg[index]; }

    hcan->pTxMsg->StdId = id;
    hcan->pTxMsg->IDE = CAN_ID_STD;
    hcan->pTxMsg->RTR = CAN_RTR_DATA;
    hcan->pTxMsg->DLC = 0x08;

    HAL_CAN_Transmit(hcan, 10);
}
