#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>

#define MOTOR_LF_IN1  PA2
#define MOTOR_LF_IN2  PA3
#define MOTOR_RF_IN1  PA8
#define MOTOR_RF_IN2  PA9
#define MOTOR_LB_IN1  PB8
#define MOTOR_LB_IN2  PB9
#define MOTOR_RB_IN1  PB6
#define MOTOR_RB_IN2  PB7

#define MOTOR_MIN_PWM 90

#define MOTOR_LF_DIR  (1)
#define MOTOR_RF_DIR  (1)
#define MOTOR_LB_DIR  (1)
#define MOTOR_RB_DIR  (1)

void Motor_Init(void);

void motorDriveLF(int16_t speed);
void motorDriveRF(int16_t speed);
void motorDriveLB(int16_t speed);
void motorDriveRB(int16_t speed);

void mecanumDrive(int16_t vx, int16_t vy, int16_t w);

#endif
