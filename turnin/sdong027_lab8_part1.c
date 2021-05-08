/*	Author: sdong027
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

enum SoundSM {OFF, C, D, E} SOUND_STATE;
double PlaySound(unsigned char A0, unsigned char A1, unsigned char A2) {
	double returnFreq = 0x00;
	switch (SOUND_STATE) {
		case OFF:
			if (A0 && !A1 && !A2) {
				SOUND_STATE = C;
			}
			else if (!A0 && A1 && !A2) {
				SOUND_STATE = D;
			}
			else if (!A0 && !A1 && A2) {
				SOUND_STATE = E;
			}
			break;
		case C:
			if (A0 && !A1 && !A2) {
				SOUND_STATE = C;
			}
			else if (!A0 && A1 && !A2) {
				SOUND_STATE = D;
			}
			else if (!A0 && !A1 && A2) {
				SOUND_STATE = E;
			}
			else {
				SOUND_STATE = OFF;
			}
			break;
		case D:
			if (A0 && !A1 && !A2) {
				SOUND_STATE = C;
			}
			else if (!A0 && A1 && !A2) {
				SOUND_STATE = D;
			}
			else if (!A0 && !A1 && A2) {
				SOUND_STATE = E;
			}
			else {
				SOUND_STATE = OFF;
			}
			break;
		case E:
			if (A0 && !A1 && !A2) {
				SOUND_STATE = C;
			}
			else if (!A0 && A1 && !A2) {
				SOUND_STATE = D;
			}
			else if (!A0 && !A1 && A2) {
				SOUND_STATE = E;
			}
			else {
				SOUND_STATE = OFF;
			}
			break;
		default:
			SOUND_STATE = OFF;
			break;	
	}
	switch (SOUND_STATE) {
		case OFF:
			PWM_off();
			returnFreq = 0.00;
			break;
		case C:
			PWM_on();
			returnFreq = 261.63;
			break;
		case D:
			PWM_on();
			returnFreq = 293.66;
			break;
		case E:
			PWM_on();
			returnFreq = 329.63;
			break;
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
