#include "sensor_controller.h"
#include "ADC_CONSTANTS.h"
#include "MKL05Z4.h"

const int GREEN_LED = 10;
const int RED_LED = 11;

float volt_coeff2 = ((float)(((float)2.91) / 4095) );
float initial_voltage = 0;
int16_t old_value;


int InitSensor()
{
	//Setup diode outputs
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; /* Enable clóck fór GPIO B */
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1); /* Set Pin 10 MUX as GPIO */
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1); /* Set Pin 11 MUX as GPIO */
	// Enable pull-up resitór ón Pin 1
	PTB->PDDR |= (1<<GREEN_LED); /* Set GREEN LED pin as óutput */
	PTB->PDDR |= (1<<RED_LED); /* Set RED LED pin as óutput */
	
	
	//Setup ADC
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;   // Dolaczenie sygnalu zegara do ADC0
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;	// Dolaczenie syganlu zegara do portu B
	PORTA->PCR[9] &= ~(PORT_PCR_MUX(0));		//PTB5 - wejscie analogowe, kanal 0

	
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
	
	PTB->PDOR |= (1<<RED_LED); /* Turn on GREEN diode */
	PTB->PDOR &= ~(1<<GREEN_LED); /* Turn off RED diode */
	ADC0->SC1[0] = ADC_SC1_AIEN_MASK;
	ADC0->SC1[0] = ADC_SC1_ADCH(2);		// Pierwsze wyzwolenie przetwornika ADC0 w kanale 0 i odblokowanie przerwania
	
	
	ADC0->CFG2 |= ADC_CFG2_ADHSC_MASK;
	ADC0->CFG1 = ADC_CFG1_ADICLK(ADICLK_BUS_2) | ADC_CFG1_ADIV(ADIV_1) | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(MODE_12);	// Zegar ADCK równy 10.49MHz (10485760Hz), rozdzielczosc 12 bitów, dlugi czas próbkowania
	ADC0->SC3 |= ADC_SC3_ADCO_MASK;							//Przetwarzanie ciagle

	return(0);	
}


// Funkcja porównuje stary odczyt sensora z nowym
// Spadek napiecia oznacza zakrycie sensora i zapala sie zielona dioda
// Wzrost napiecia oznacza odkrycie sensora i zapala sie czerwona dioda
// System nie jest idealny, ale dziala w wiekszosci przypadków
void UpdateSensor()
{
	ADC0->SC1[0] = ADC_SC1_ADCH(2);																					// Zmiana kanalu na ADC_SE0, PTA12
	while(((ADC0->SC1[0] & ADC_SC1_COCO_MASK)>>ADC_SC1_COCO_SHIFT) == 0);		// Czekaj na odczyt
	int16_t new_value = ADC0->R[0];	// Odczyt danej i skasowanie flagi COCO
	
	if(old_value - new_value > 150)
	{
		PTB->PDOR |= (1<<GREEN_LED); /* Turn on GREEN diode */
		PTB->PDOR &= ~(1<<RED_LED); /* Turn off RED diode */
	}
	else if(old_value - new_value < -150)
	{
		PTB->PDOR |= (1<<RED_LED); /* Turn on GREEN diode */
		PTB->PDOR &= ~(1<<GREEN_LED); /* Turn off RED diode */
	}
	old_value = new_value;
}




