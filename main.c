#include "MKL05Z4.h"

#include "frdm_bsp.h"
#include "lcd1602.h"
#include "analog_controller.h"
#include "servo_controller.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const float SENSITIVITY = 20.f;

static uint8_t newTick = 0;
uint32_t value = 900;

int main (void)
{
	uint8_t	kal_error;
	char display[300];
	LCD1602_Init();		 // Inicjalizacja wyswietlacza LCD
	LCD1602_Backlight(TRUE);
	LCD1602_Print("---");	
	
	kal_error = InitController();
	while(kal_error) { }
	
	SysTick_Config(480000);
	InitServo();
	
	while(1)
	{
		if(newTick)
		{
			float value_x = GetValueX(); // Odczyt ADC, zwraca wartość -1.0 do 1.0
			AddInputX(value_x * 0.001f * SENSITIVITY); // Zmienia długość impulsu serwa
			
			newTick = 0;
		}
		

	}
}

void SysTick_Handler(void) {
	newTick = 1;
}
