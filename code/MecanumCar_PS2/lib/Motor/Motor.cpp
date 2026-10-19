#include "Motor.h"

void Motor_Init(void) {
  pinMode(MOTOR_LF_IN1, OUTPUT);
  pinMode(MOTOR_LF_IN2, OUTPUT);
  pinMode(MOTOR_RF_IN1, OUTPUT);
  pinMode(MOTOR_RF_IN2, OUTPUT);
  pinMode(MOTOR_LB_IN1, OUTPUT);
  pinMode(MOTOR_LB_IN2, OUTPUT);
  pinMode(MOTOR_RB_IN1, OUTPUT);
  pinMode(MOTOR_RB_IN2, OUTPUT);
}

static void stopPwm(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

static void driveOne(uint8_t in1, uint8_t in2, int16_t speed) {
  speed = constrain(speed, -255, 255);
  if (speed > 0 && speed < MOTOR_MIN_PWM) speed = MOTOR_MIN_PWM;
  if (speed < 0 && speed > -MOTOR_MIN_PWM) speed = -MOTOR_MIN_PWM;
  if (speed > 0) {
    stopPwm(in2);
    analogWrite(in1, speed);
  } else if (speed < 0) {
    stopPwm(in1);
    analogWrite(in2, -speed);
  } else {
    stopPwm(in1);
    stopPwm(in2);
  }
}

void motorDriveLF(int16_t speed) { driveOne(MOTOR_LF_IN1, MOTOR_LF_IN2, speed * MOTOR_LF_DIR); }
void motorDriveRF(int16_t speed) { driveOne(MOTOR_RF_IN1, MOTOR_RF_IN2, speed * MOTOR_RF_DIR); }
void motorDriveLB(int16_t speed) { driveOne(MOTOR_LB_IN1, MOTOR_LB_IN2, speed * MOTOR_LB_DIR); }
void motorDriveRB(int16_t speed) { driveOne(MOTOR_RB_IN1, MOTOR_RB_IN2, speed * MOTOR_RB_DIR); }

#define MOTOR_SPEED_NUM  3
#define MOTOR_SPEED_DEN  5
#define MOTOR_RAMP     0.7f
#define STRAFE_REAR_PCT  60

static int16_t rampTo(int16_t cur, int16_t target) {
  if (cur == target) return cur;
  int16_t d = (int16_t)((target - cur) * MOTOR_RAMP);
  if (d == 0) d = (target > cur) ? 1 : -1;
  return cur + d;
}

static int16_t V_LF = 0, V_RF = 0, V_LB = 0, V_RB = 0;

void mecanumDrive(int16_t vx, int16_t vy, int16_t w) {
  const int16_t vyR = vy * STRAFE_REAR_PCT / 100;
  V_LF = rampTo(V_LF, (vx - vy  - w) * MOTOR_SPEED_NUM / MOTOR_SPEED_DEN);
  V_RF = rampTo(V_RF, (vx + vy  + w) * MOTOR_SPEED_NUM / MOTOR_SPEED_DEN);
  V_LB = rampTo(V_LB, (vx + vyR - w) * MOTOR_SPEED_NUM / MOTOR_SPEED_DEN);
  V_RB = rampTo(V_RB, (vx - vyR + w) * MOTOR_SPEED_NUM / MOTOR_SPEED_DEN);

  motorDriveLF(V_LF);
  motorDriveRF(V_RF);
  motorDriveLB(V_LB);
  motorDriveRB(V_RB);
}
