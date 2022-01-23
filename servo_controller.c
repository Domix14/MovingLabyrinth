#include "servo_controller.h"
#include "utils.h"

const int32_t MIN_CNV_X = 432;
const int32_t MAX_CNV_X = 532;
const int32_t MID_CNV_X = 482;
float axis_x_value = 0.5f; //0.f <-> 1.f 

const int32_t MIN_CNV_Y = 500;
const int32_t MAX_CNV_Y = 590;
const int32_t MID_CNV_Y = 545;
float axis_y_value = 0.5f; // 0.f <-> 1.f


void InitServo()
{
	// Konfiguracja PWM dla serw
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;		// Enable TPM0 mask in SCGC6 register
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);  // Choose MCGFLLCLK clock source

	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; // Connect port B to clock
	PORTB->PCR[9] = PORT_PCR_MUX(2);		// Set multiplekser to TPM0 for PTB9, get channel number (page 148 of the Reference Manual)
	PORTB->PCR[8] = PORT_PCR_MUX(2);
	
	TPM0->SC |= TPM_SC_PS(7);  					// Set prescaler to 128
	TPM0->SC |= TPM_SC_CMOD(1);					// For TMP0, select the internal input clock source

	TPM0->MOD = 7500; 										// Set MOD=7500, 50 Hz
	
	TPM0->SC &= ~TPM_SC_CPWMS_MASK; 		/* up counting */
	TPM0->CONTROLS[2].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK); /* set Output on match, clear Output on reload */ 
	TPM0->CONTROLS[3].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK); /* set Output on match, clear Output on reload */ 
	
	//Reset planszy
	ResetRotation();
}


float AddInputX(float value)
{
	axis_x_value = clamp(axis_x_value + value, 0.f, 1.f);
	TPM0->CONTROLS[2].CnV = (axis_x_value * (MAX_CNV_X - MIN_CNV_X)) + MIN_CNV_X; //PTB9
	return axis_x_value;
}


float AddInputY(float value)
{
	axis_y_value = clamp(axis_y_value + value, 0.f, 1.f);
	TPM0->CONTROLS[3].CnV = (axis_y_value * (MAX_CNV_Y - MIN_CNV_Y)) + MIN_CNV_Y; //PTB8
	return axis_y_value;
}

// Resetuje rotacje planszy
void ResetRotation()
{
	axis_x_value = 0.5f;
	axis_y_value = 0.5f;
	TPM0->CONTROLS[2].CnV = MID_CNV_X; 				// Set starting value to neutral
	TPM0->CONTROLS[3].CnV = MID_CNV_Y; 				// Set starting value to neutral
}
