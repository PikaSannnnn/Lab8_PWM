/*	Author: sdong027
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #3
 *	Exercise Description: Source for sheet music: https://musescore.com/punctuationless/never-gonna-give-you-up
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

// Melody Arrays
const int totNotes = 16;		// total Frequencies
const double melody[] = {349.23, 392.00, 261.63, 392.00, 440.00, 523.25, 493.88, 440.00, 349.23, 392.00, 261.63, 261.63, 261.63, 293.66, 329.63, 349.23};
const double noteTime[] = {325, 625, 275, 325, 425, 175, 175, 225, 325, 625, 525, 175, 175, 175, 225, 225}	// total = 5000ms, max = 5000ms

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff() {
	TCCR1B = 0x00;
}
void TimerISR() {
	TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

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

enum SoundSM {WAIT, PLAY, DEC, INC} SOUND_STATE;
double PlaySound(unsigned char input) {
	static int index;
	static unsigned char prevInput;
	switch (SOUND_STATE) {
		case WAIT:
			if (input) {
				SOUND_STATE = TOGGLE;
			}
			break;
		case WAIT_PRESS:
			if (prevInput == input) {	// same input
				SOUND_STATE = WAIT_PRESS;
			}
			else {				// not same input (change to different state)
				if (input == 0x00) {	// all not pressed
					SOUND_STATE = WAIT;
				}
				else if (input == 0x01) {	// only toggle pressed
					SOUND_STATE = TOGGLE;
				}
				else if (input == 0x02) {	// only dec pressed
					SOUND_STATE = DEC;
				}
				else if (input == 0x04) {	// only inc pressed
					SOUND_STATE = INC;
				}
				// multiple buttons pressed -> nothing
			}
			prevInput = input;
			break;
		case TOGGLE:
			SOUND_STATE = WAIT_PRESS;
			break;
		case DEC:
			SOUND_STATE = WAIT_PRESS;
			break;
		case INC:
			SOUND_STATE = WAIT_PRESS;
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
		case WAIT_PRESS:
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
			if (index > 0) {	// check bound
				index--;			
			}
			break;
		case INC:
			if (index < (totFrequencies - 1)) {	// check bound
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
		inFreq = PlaySound(input);	// 0001, 0010, 0100
		set_PWM(inFreq);
	}
	return 1;
}
