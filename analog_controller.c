

#include "analog_controller.h"
#include "ADC_CONSTANTS.h"
#include <math.h>
#include "utils.h"

const float HALF_V_REF_Y = 1.5f;
const float HALF_V_REF_X = 1.5f;
float volt_coeff = ((float)(((float)2.91) / 4095) );
float value_x = 0.f;
float value_y = 0.f;


uint8_t InitController()
{
	// Konfiguracja ADC 
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;          // Dolaczenie sygnalu zegara do ADC0
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;	// Dolaczenie syganlu zegara do portu A
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;	// Dolaczenie syganlu zegara do portu B
	PORTA->PCR[12] &= ~(PORT_PCR_MUX(0));		//PTA12 - wejscie analogowe, kanal 0
	PORTB->PCR[1] &= ~(PORT_PCR_MUX(0));		//PTB1 - wejscie analogowe, kanal 0

	
	ADC0->CFG1 = ADC_CFG1_ADICLK(ADICLK_BUS_2) | ADC_CFG1_ADIV(ADIV_4) | ADC_CFG1_ADLSMP_MASK;	// Zegar ADCK równy 2.62MHz (2621440Hz)
	ADC0->CFG2 = ADC_CFG2_ADHSC_MASK;		// Wlacz wspomaganie zegara o duzej czestotliwosci
	ADC0->SC3  = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3);		// Wlacz usrednianie na 32
	
	ADC0->SC3 |= ADC_SC3_CAL_MASK;				// Rozpoczecie kalibracji
	while(((ADC0->SC1[0] & ADC_SC1_COCO_MASK)>>ADC_SC1_COCO_SHIFT) == 0);		// Czekaj na koniec kalibracji
	
	if(ADC0->SC3 & ADC_SC3_CALF_MASK)
{
	  ADC0->SC3 |= ADC_SC3_CALF_MASK;
	  return(1);								// Wróc, jesli blad kalibracji
	}
	
	ADC0->SC1[0] = ADC_SC1_AIEN_MASK;
	ADC0->SC1[0] = ADC_SC1_ADCH(0);		// Pierwsze wyzwolenie przetwornika ADC0 w kanale 0 i odblokowanie przerwania
	
	ADC0->CFG2 |= ADC_CFG2_ADHSC_MASK;
	ADC0->CFG1 = ADC_CFG1_ADICLK(ADICLK_BUS_2) | ADC_CFG1_ADIV(ADIV_1) | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(MODE_12);	// Zegar ADCK równy 10.49MHz (10485760Hz), rozdzielczosc 12 bitów, dlugi czas próbkowania
	ADC0->SC3 |= ADC_SC3_ADCO_MASK;							//Przetwarzanie ciagle
	
	// Konfiguracja guzika
	PORTB->PCR[13] |= PORT_PCR_MUX(1); /* Set Pin 1 MUX as GPIO */
	PORTB->PCR[13] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	
	return(0);
}

// Funkcje zwracaja wartosc w przedziale od -1.f do 1.f w zaleznosci od wychylenia galki
float GetValueX()
{
	ADC0->SC1[0] = ADC_SC1_ADCH(0);																					// Zmiana kanalu na ADC_SE0, PTA12
	while(((ADC0->SC1[0] & ADC_SC1_COCO_MASK)>>ADC_SC1_COCO_SHIFT) == 0);		// Czekaj na odczyt
	uint16_t adc_value_x = ADC0->R[0];	// Odczyt danej i skasowanie flagi COCO
	value_x = adc_value_x * volt_coeff;
	value_x = clamp((((adc_value_x * volt_coeff) - HALF_V_REF_X) * 1 / HALF_V_REF_X), -1.f, 1.f);
	if(fabs(value_x) > 0.2) return value_x;
	return 0.f;
}

float GetValueY()
{
	ADC0->SC1[0] = ADC_SC1_ADCH(5);																					// Zmiana kanalu na ADC_SE5, PTB1
	while(((ADC0->SC1[0] & ADC_SC1_COCO_MASK)>>ADC_SC1_COCO_SHIFT) == 0);	  // Czekaj na odczyt
	uint16_t adc_value_y = ADC0->R[0];	// Odczyt danej i skasowanie flagi COCO
	value_y = clamp((((adc_value_y * volt_coeff) - HALF_V_REF_Y) * 1 / HALF_V_REF_Y), -1.f, 1.f);
	if(fabs(value_y) > 0.2) return value_y;
	return 0.f;
}

// Zwraca prawde, jezeli guzik jest wcisniety
uint16_t IsPressed()
{
	return ( ( PTB->PDIR & (1<<13) ) ==0 );
}
	


