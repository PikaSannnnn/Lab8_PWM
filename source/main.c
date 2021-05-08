/*	Author: sdong027
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char PWM_status = 0x00;	// flag for PWM
const int totFrequencies = 8;		// total Frequencies
const double frequencies[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};	// Array of all frequencies

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }

		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
	}
}
void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}
void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum SoundSM {WAIT, WAIT_TOGGLE, WAIT_DEC, WAIT_INC, TOGGLE, DEC, INC} SOUND_STATE;
double PlaySound(unsigned char toggleButton, unsigned char decButton, unsigned char incButton) {
	static int index;
	switch (SOUND_STATE) {
		case WAIT:
			if (toggleButton && !decButton && !incButton) {
				SOUND_STATE = TOGGLE;
			}
			else if (!toggleButton && decButton && !incButton) {
				SOUND_STATE = DEC;
			}
			else if (!toggleButton && !decButton && incButton) {
				SOUND_STATE = INC;
			}
			break;
		case WAIT_TOGGLE:
			if (!toggleButton && !decButton && !incButton) {	// all not pressed
				SOUND_STATE = WAIT;
			}
			else if (!toggleButton && decButton && !incButton) {
				SOUND_STATE = DEC;
			}
			else if (!toggleButton && !decButton && incButton) {
				SOUND_STATE = INC;
			}
			break;
		case WAIT_DEC:
			if (!toggleButton && !decButton && !incButton) {	// all not pressed
				SOUND_STATE = WAIT;
			}
			else if (toggleButton && !decButton && !incButton) {
				SOUND_STATE = TOGGLE;
			}
			else if (!toggleButton && !decButton && incButton) {
				SOUND_STATE = INC;
			}
			break;
		case WAIT_INC:
			if (!toggleButton && !decButton && !incButton) {	// all not pressed
				SOUND_STATE = WAIT;
			}
			else if (toggleButton && !decButton && !incButton) {
				SOUND_STATE = TOGGLE;
			}
			else if (!toggleButton && decButton && !incButton) {
				SOUND_STATE = DEC;
			}
			break;
		case TOGGLE:
			SOUND_STATE = WAIT_TOGGLE;
			break;
		case DEC:
			SOUND_STATE = WAIT_DEC;
			break;
		case INC:
			SOUND_STATE = WAIT_INC;
			break;
		default:
			SOUND_STATE = WAIT;
			index = 0;
			PWM_off();
			break;	
	}
	switch (SOUND_STATE) {
		case WAIT:
			break;
		case WAIT_TOGGLE:
			break;
		case WAIT_DEC:
			break;
		case WAIT_INC:
			break;
		case TOGGLE:
			if (PWM_status) {
				PWM_off();
			}
			else {
				PWM_on();
			}
			PWM_status = ~PWM_status;
			break;
		case DEC:
			if (index > 0) {
				index--;			
			}
			break;
		case INC:
			if (index < totFrequencies) {
				index++;			
			}
			break;
	}
	return frequencies[index];
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned char input = 0x00;
	double inFreq = 0x00;
	
	/* Insert your solution below */
	while (1) {
		input = ~PINA & 0x07;
		inFreq = PlaySound((input & 0x01), (input & 0x02), (input & 0x04));	// 0001, 0010, 0100
		set_PWM(inFreq);
	}
	return 1;
}
