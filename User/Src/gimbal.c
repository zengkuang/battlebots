//
// Created by beck on 19/10/2017.
//

#include "gimbal.h"
#include "chassis_motors.h" // Need to get the CAN bus out of the chassis_motors later
#include "test_imu.h"
#include "arm_math.h"
#include "main.h"

extern volatile Encoder CM5Encoder;
extern volatile Encoder CM6Encoder;
extern arm_pid_instance_f32 YawPositionPID;
extern arm_pid_instance_f32 YawVelocityPID;
extern arm_pid_instance_f32 PitchPositionPID;
extern arm_pid_instance_f32 PitchVelocityPID;
extern IMUDataTypedef imu_data;

void control_pitch_yaw(int16_t target_angle_pitch, int16_t target_angle_yaw)
{
    // CAN 0x205
    float yaw_position_target = arm_pid_f32(&YawPositionPID, (float32_t)(target_angle_yaw - CM5Encoder.position_raw_value));
    float yaw_velocity_target = arm_pid_f32(&YawVelocityPID, (float32_t)(yaw_position_target - imu_data.gz));

    // CAN 0x206
    float pitch_position_target = arm_pid_f32(&PitchPositionPID, (float32_t)(target_angle_pitch - CM6Encoder.position_raw_value));
    float pitch_velocity_target = arm_pid_f32(&PitchVelocityPID, (float32_t)(pitch_position_target - imu_data.gy));

}