#include "MKL05Z4.h"

#include "frdm_bsp.h"
#include "lcd1602.h"
#include "analog_controller.h"
#include "servo_controller.h"
#include "sensor_controller.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const float SENSITIVITY = 15.f;

static uint8_t newTick = 0;
uint32_t value = 900;

int main (void)
{	
	SysTick_Config(480000);
	
	InitController();
	InitServo();
	InitSensor();
	
	while(1)
	{
		if(newTick)
		{
			float value_y = GetValueY(); // Odczyt ADC, zwraca wartość -1.0 do 1.0
			AddInputY(value_y * 0.001f * SENSITIVITY); // Zmienia długość impulsu serwa
			
			float value_x = GetValueX(); // Odczyt ADC, zwraca wartość -1.0 do 1.0
			AddInputX(value_x * 0.001f * SENSITIVITY); // Zmienia długość impulsu serwa
			
			if(IsPressed()) ResetRotation(); // Resetuje plansze gdy analog jest wciśnięty
			
			UpdateSensor(); // Sprawdza odczyt na sensorze i aktualizuję kolor diody
			
			newTick = 0;
		}
		

	}
}

void SysTick_Handler(void) {
	newTick = 1;
}
