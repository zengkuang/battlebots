/**
  *@file test_can.c
  *@date 2016-12-12
  *@author Albert.D
  *@brief 
  */

#include <chassis_motors.h>
#include "can.h"
#include "arm_math.h"
#include "chassis_motors.h"

  
uint8_t can1_rx_data[8];
uint8_t can2_rx_data[8];

// Chassis Motor Encoder
volatile Encoder CM1Encoder = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // CAN Address 201
volatile Encoder CM2Encoder = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // CAN Address 202
volatile Encoder CM3Encoder = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // CAN Address 203
volatile Encoder CM4Encoder = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // CAN Address 204

uint32_t can_chassis_count[4] = {0, 0, 0, 0};

extern arm_pid_instance_q15 CMotorPID;

int16_t CM1velocity;
int16_t CM2velocity;
int16_t CM3velocity;
int16_t CM4velocity;

/*
 * can filter must be initialized before use
 */
void CanFilter_Init(CAN_HandleTypeDef* hcan)
{
    CAN_FilterConfTypeDef canfilter;

    //create memory to save the message, if not will raise error
    static CanTxMsgTypeDef  Tx1Message;
    static CanRxMsgTypeDef  Rx1Message;
    static CanTxMsgTypeDef  Tx2Message;
    static CanRxMsgTypeDef  Rx2Message;

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
    if(hcan == &hcan1)
    {
        canfilter.FilterNumber = 0;
        hcan->pTxMsg = &Tx1Message;
        hcan->pRxMsg = &Rx1Message;
    }
    if(hcan == &hcan2)
    {
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
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    if (hcan == &hcan1) {       // CAN bus 1 is for drive motors
        if  (hcan->pRxMsg->IDE == CAN_ID_STD) {  // Check correct CAN Identifier Type

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
void encoderProcess(volatile Encoder* ecd, CanRxMsgTypeDef* msg)
{
    // Initialize the angle
    ecd->position_raw_value_last = ecd->position_raw_value;

    // Bitwise OR operation to extract the 16-bit information
    ecd->velocity_from_ESC  = (msg->Data[2] << 8) | msg->Data[3];
    ecd->position_raw_value = (msg->Data[0] << 8) | msg->Data[1];
}

/*
 * PID Closed loop speed controller for chassis motors
 * Input  target speed
 * Output CAN bus command for the motor current
 */
void Chassis_Set_Speed(int16_t RPM1, int16_t RPM2, int16_t RPM3, int16_t RPM4)
{
    CM1velocity = arm_pid_q15(&CMotorPID, RPM1 - CM1Encoder.velocity_from_ESC) + RPM1;
    CM2velocity = arm_pid_q15(&CMotorPID, RPM2 - CM2Encoder.velocity_from_ESC) + RPM2;
    CM3velocity = arm_pid_q15(&CMotorPID, RPM3 - CM3Encoder.velocity_from_ESC) + RPM3;
    CM4velocity = arm_pid_q15(&CMotorPID, RPM4 - CM4Encoder.velocity_from_ESC) + RPM4;


    uint8_t can1_message_chassis[8] = {0,0,0,0,0,0,0,0};

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
void CAN_Send_Msg(CAN_HandleTypeDef* hcan, uint8_t *msg, uint32_t id)
{
    uint8_t index = 0;
    for(index = 0; index < 8; index++) {hcan->pTxMsg->Data[index] = msg[index];}

    hcan->pTxMsg->StdId = id;
    hcan->pTxMsg->IDE = CAN_ID_STD;
    hcan->pTxMsg->RTR = CAN_RTR_DATA;
    hcan->pTxMsg->DLC = 0x08;

    HAL_CAN_Transmit(hcan, 10);
}
