#ifndef ARM_H
#define ARM_H
#include <Arduino.h>
#include <PS2.h>

void Arm_Init(void);
void Arm_Update(const stPS2KeyValue* ps2);
void Arm_Service(void);

#endif
