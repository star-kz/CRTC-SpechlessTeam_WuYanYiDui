#include <Arduino.h>
#include <PS2.h>
#include <Motor.h>
#include <Arm.h>

#define CTRL_PERIOD_MS  40
#define TURN_SPEED      255

stPS2KeyValue* ps2 = NULL;
unsigned long lastCtrl = 0;

int16_t int_Lerp(int16_t a, int16_t b, float t) {
  return (int16_t)(a + (b - a) * t);
}

int16_t stickToSpeed(uint8_t raw) {
  int16_t ori = (128 - (int16_t)raw) * 2;
  if (abs(ori) < 6) return 0;
  if (abs(ori) < 30)
    return int_Lerp(0, 80, (float)(abs(ori) - 6) / 24.0f) * abs(ori) / ori;
  return int_Lerp(80, 255, (float)(abs(ori) - 30) / 98.0f) * abs(ori) / ori;
}

// #define DEBUG_PS2  0

void setup() {
// #if DEBUG_PS2
//   Serial.begin(115200);
// #endif
  PS2_Init();
  Motor_Init();
  Arm_Init();
}

void loop() {
  Arm_Service();

  unsigned long now = millis();
  if (now - lastCtrl < CTRL_PERIOD_MS) return;
  lastCtrl = now;

  ps2 = ReadPS2KeyValue();
  if (ps2 == NULL) return;

// #if DEBUG_PS2
//   if (ps2->FunBtn != 0xFFFF) {
//     Serial.print("FunBtn=0x");
//     Serial.println(ps2->FunBtn, HEX);
//   }
// #endif

  int16_t vx = stickToSpeed(ps2->Y1);
  int16_t vy = -stickToSpeed(ps2->X1);

  int16_t w = 0;
  bool turnLeft  = (ps2->FunBtn & PSB_L2) == 0;
  bool turnRight = (ps2->FunBtn & PSB_R2) == 0;
  if (turnLeft && !turnRight)  w = TURN_SPEED;
  if (turnRight && !turnLeft)  w = -TURN_SPEED;

  mecanumDrive(vx, vy, w);

  Arm_Update(ps2);
}
