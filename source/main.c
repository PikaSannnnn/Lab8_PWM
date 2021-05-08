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

const int totFrequencies = 8;
const double frequencies[totFrequencies] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

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

enum SoundSM {WAIT, WAIT_PRESS, TOGGLE, DEC, INC} SOUND_STATE;
double PlaySound(unsigned char toggleButton, unsigned char decButton, unsigned char incButton) {
	double returnFreq = 0x00;
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
		case WAIT_PRESS:
			if (!toggleButton && !decButton && !incButton) {
				SOUND_STATE = WAIT;
			}
			else if (toggleButton && !decButton && !incButton) {
				SOUND_STATE = TOGGLE;
			}
			else if (!toggleButton && decButton && !incButton) {
				SOUND_STATE = DEC;
			}
			else if (!toggleButton && !decButton && incButton) {
				SOUND_STATE = INC;
			}
			break;
		default:
			SOUND_STATE = WAIT;
			index = 0;
			PWM_off();
			break;	
	}
	switch (SOUND_STATE) {
		
	}

	return returnFreq;
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
