#ifndef ANALOG_CONTROLLER_H
#define ANALOG_CONTROLLER_H

#include "MKL05Z4.h"



uint8_t InitController(void);

float GetValueX(void);
float GetValueY(void);
uint16_t IsPressed(void);

#endif
