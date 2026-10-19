#include "PS2.h"

#define PS2_BIT_DELAY_US    5
#define PS2_BYTE_GAP_US     120
#define PS2_ATT_DELAY_US    120
#define PS2_FRAME_GAP_MS    10

static uint8_t ps2_transfer(uint8_t out) {
  uint8_t in = 0;
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(PS2_PIN_CMD, (out & 0x01) ? HIGH : LOW);
    out >>= 1;
    digitalWrite(PS2_PIN_CLK, LOW);
    delayMicroseconds(PS2_BIT_DELAY_US);
    if (digitalRead(PS2_PIN_DAT)) in |= (1 << i);
    digitalWrite(PS2_PIN_CLK, HIGH);
    delayMicroseconds(PS2_BIT_DELAY_US);
  }
  digitalWrite(PS2_PIN_CMD, HIGH);
  delayMicroseconds(PS2_BYTE_GAP_US);
  return in;
}

static void ps2_frame(const uint8_t* tx, uint8_t* rx, uint8_t n) {
  digitalWrite(PS2_PIN_CS, LOW);
  delayMicroseconds(PS2_ATT_DELAY_US);
  for (uint8_t i = 0; i < n; i++) {
    uint8_t r = ps2_transfer(tx[i]);
    if (rx) rx[i] = r;
  }
  digitalWrite(PS2_PIN_CS, HIGH);
  delay(PS2_FRAME_GAP_MS);
}

void PS2_Init(void) {
  pinMode(PS2_PIN_DAT, INPUT_PULLUP);
  pinMode(PS2_PIN_CMD, OUTPUT);
  pinMode(PS2_PIN_CS,  OUTPUT);
  pinMode(PS2_PIN_CLK, OUTPUT);
  digitalWrite(PS2_PIN_CS,  HIGH);
  digitalWrite(PS2_PIN_CLK, HIGH);
  digitalWrite(PS2_PIN_CMD, HIGH);
  delay(100);

  static const uint8_t scan[]      = {0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  static const uint8_t enter_cfg[] = {0x01,0x43,0x00,0x01,0x00};
  static const uint8_t set_mode[]  = {0x01,0x44,0x00,0x01,0x03,0x00,0x00,0x00,0x00};
  static const uint8_t set_len[]   = {0x01,0x4f,0x00,0xff,0xff,0x03,0x00,0x00,0x00};
  static const uint8_t exit_cfg[]  = {0x01,0x43,0x00,0x00,0x5A,0x5A,0x5A,0x5A,0x5A};
  uint8_t tmp[9];

  ps2_frame(scan, tmp, 9);
  ps2_frame(scan, tmp, 9);
  ps2_frame(enter_cfg, NULL, 5);
  ps2_frame(set_mode, NULL, 9);
  ps2_frame(set_len,  NULL, 9);
  ps2_frame(exit_cfg, NULL, 9);
  ps2_frame(scan, tmp, 9);
}

stPS2KeyValue* ReadPS2KeyValue(void) {
  static stPS2KeyValue key = {128, 128, 128, 128, 0xFFFF};
  static const uint8_t scan[] = {0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  uint8_t out[9] = {0};

  digitalWrite(PS2_PIN_CS, LOW);
  delayMicroseconds(PS2_ATT_DELAY_US);
  for (uint8_t i = 0; i < 9; i++) out[i] = ps2_transfer(scan[i]);
  digitalWrite(PS2_PIN_CS, HIGH);

  if (out[1] != 0x41 && out[1] != 0x73 && out[1] != 0x79) return &key;
  if (out[3] == 0x00 && out[4] == 0x00 &&
      out[5] == 0x00 && out[6] == 0x00 && out[7] == 0x00 && out[8] == 0x00) return &key;

  key.FunBtn = ((uint16_t)out[4] << 8) | out[3];
  key.X2 = out[5];
  key.Y2 = out[6];
  key.X1 = out[7];
  key.Y1 = out[8];
  return &key;
}
