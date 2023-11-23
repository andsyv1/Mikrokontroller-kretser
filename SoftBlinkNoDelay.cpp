/*
 * SoftblinkNoDelay.cpp
 *
 * Created: 15.11.2023 13:22:18
 * Author : Truls Kolstad
 *
 * Denne koden styrer dimmingen til en LED ved bruk av
 * interrupt handleren til microchippen. Lysstyrken til 
 * LED-en styres av verdien pwmValue, som gradvis
 * inkrementerer opp eller ned i takt med frekvensen til
 * timeren.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

// Definerer pin som LED er koplet til
#define ledPin PD6

// Startverdi til PWM komparator
volatile uint8_t pwmValue = 0; 
// Inkrementet til PWM komparator
volatile int8_t increment = 1;

void pwmInit() {
	// Konfigurer PD6/OC0A som en utgang. 
	DDRD |= (1 << ledPin);

	// Konfigurer Timer/Counter Control Register 0A til 
	// Fast PWM mode med ikke-invertert utgang.
	TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A1);
	
	// De ulike prescaler-verdiene til timer/counter0, og resulterende frekvens.
	//TCCR0B = (1 << CS00);								 // 0 timer prescaler @ 3.9kHz
	TCCR0B |= (1 << CS01);								 // 8 timer prescaler @ 488 Hz
	//TCCR0B |= (1 << CS01) | (1 << CS00);				 // 64 timer prescaler @ 61 Hz
	//TCCR0B |= (1 << CS02) | (0 << CS01) | (0 << CS00); // 256 timer prescaler @ 15 Hz
	//TCCR0B |= (1 << CS02) | (0 << CS01) | (1 << CS00); // 1024 timer prescaler @ 4 Hz
	
	// Startverdi sendt til OCR0A komparator
	OCR0A = 128;
	
	// Skrur på Timer/Counter0 Overflow interrupt
	// Altså en interrupt varsles når timeren er på topp.
	TIMSK0 |= (1 << TOIE0);
	
	// Skrur på globale interrupts
	sei();
}

// Dette er funskjonen som kjører for hver gang interrupt blir trigget, som
// skjer ved hver timer0 overflow. 
ISR(TIMER0_OVF_vect) {
	// Setter verdi til OCR0A, som betyr PD6 er HØY helt til timer0 == OCR0A 
	OCR0A = pwmValue;

	// Øker/minker PWM verdien med et inkrement
	pwmValue += increment;

	// Snur retningen på inkrementet, så dioden går fra lysere til mørkere, eller motsatt.
	// Skjer når PWM har nådd maks eller min oppløsning. 
	if (pwmValue == 0 || pwmValue == 255) {
		increment = -increment;
	}
}

int main(void) {
	// Kun initialiseringen skjer her, da SEI håndterer interrupts internt i microcontrolleren.
	pwmInit();

	while (1) {
		
	}

	return 0;
}
