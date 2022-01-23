#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include "frdm_bsp.h"

void InitServo(void);
float AddInputX(float value);
float AddInputY(float value);
void ResetRotation(void);

#endif
