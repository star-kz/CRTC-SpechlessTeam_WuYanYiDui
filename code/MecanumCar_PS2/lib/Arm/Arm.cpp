#include "Arm.h"
#include <Servo.h>

#define SERVO1_PIN  PA0
#define SERVO2_PIN  PA1
#define SERVO3_PIN  PB12
#define SERVO4_PIN  PB13
#define SERVO5_PIN  PB14

#define STEP_CLAW    40
#define STEP_WRIST   40
#define STEP_ARM3    24
#define STEP_ARM4    40
#define STEP_BASE    64
#define STEP_TRIM3   16

#define SERVO_STEP_BUDGET 80

#define ARM_STICK_DEADZONE 24

#define SERVO_QUIET_BAND 3

#define SERVO_REFRESH_MS 4

static Servo   servos[5];
static int16_t servoAngle[5] = {900, 900, 900, 900, 900};

static int16_t stickToServoStep(uint8_t raw, int16_t maxStep) {
  int16_t off = (int16_t)raw - 128;
  if (off > -ARM_STICK_DEADZONE && off < ARM_STICK_DEADZONE) return 0;
  return off * maxStep / 128;
}

void Arm_Init(void) {
  servos[0].attach(SERVO1_PIN);
  delay(5);
  servos[1].attach(SERVO2_PIN);
  delay(5);
  servos[2].attach(SERVO3_PIN);
  delay(5);
  servos[3].attach(SERVO4_PIN);
  delay(5);
  servos[4].attach(SERVO5_PIN);
  for (uint8_t i = 0; i < 5; i++)
  {
    servos[i].write(servoAngle[i] / 10);
    delay(40);
  }
}

void Arm_Update(const stPS2KeyValue* ps2) {
  uint16_t btn = ps2->FunBtn;
  int16_t step[5] = {0, 0, 0, 0, 0};

  if ((btn & PSB_L1) == 0)     step[0] -= STEP_CLAW;
  if ((btn & PSB_R1) == 0)     step[0] += STEP_CLAW;
  if ((btn & PSB_SQUARE) == 0) step[1] -= STEP_WRIST;
  if ((btn & PSB_CIRCLE) == 0) step[1] += STEP_WRIST;

  step[4] = stickToServoStep(ps2->X2, STEP_BASE);
  step[2] = stickToServoStep(ps2->Y2, STEP_ARM3);
  step[3] = stickToServoStep(ps2->Y2, STEP_ARM4);

  if ((btn & PSB_TRIANGLE) == 0) step[2] += STEP_TRIM3;
  if ((btn & PSB_CROSS) == 0)    step[2] -= STEP_TRIM3;

  int16_t total = 0;
  for (uint8_t i = 0; i < 5; i++) total += abs(step[i]);
  if (total > SERVO_STEP_BUDGET) {
    for (uint8_t i = 0; i < 5; i++) step[i] = step[i] * SERVO_STEP_BUDGET / total;
  }

  for (uint8_t i = 0; i < 5; i++) {
    servoAngle[i] = constrain(servoAngle[i] + step[i], 0, 1800);
  }

  if ((btn & PSB_START) == 0) {
    for (uint8_t i = 0; i < 5; i++) servoAngle[i] = 900;
  }
}

void Arm_Service(void) {
  static unsigned long lastRefresh = 0;
  static uint8_t idx = 0;
  static int16_t lastWritten[5] = {-1, -1, -1, -1, -1};
  unsigned long now = millis();
  if (now - lastRefresh < SERVO_REFRESH_MS) return;
  lastRefresh = now;

  int16_t deg = servoAngle[idx] / 10;
  if (deg != lastWritten[idx]) {
    servos[idx].write(deg);
    lastWritten[idx] = deg;
  }
  idx = (idx + 1) % 5;
}
