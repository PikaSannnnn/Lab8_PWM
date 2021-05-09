/*	Author: sdong027
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #3
 *	Exercise Description: Sound component is a little faulty: There are random inconsistent scratches here and there (uncontrollable). 
 *						  Demo is the one with least amount of scratches I was able to record.
 *	Link to Demo: https://www.youtube.com/watch?v=Z4PzYR-OYXg
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
const int totNotes = 14;		// total Frequencies
const double melody[] = {261.63, 261.63, 392.00, 392.00, 440.00, 440.00, 392.00, 349.23, 349.23, 329.63, 329.63, 293.66, 293.66, 261.63};
const double noteTime[] = {300, 300, 300, 300, 300, 300, 350, 300, 300, 300, 300, 300, 300, 350};	// Total: 5000ms (with pauses)

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

void Pause(int i) {
	for (i = 0; i < 3; i++) {
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
enum SoundSM {WAIT, WAIT_PRESS, PLAY} SOUND_STATE;
double PlaySound(unsigned char input) {
	static int index;	// for arrays
	static int i;		// for counting
	switch (SOUND_STATE) {
		case WAIT:
			if (input) {
				SOUND_STATE = PLAY;
				index = 0;
				i = 0;
			}
			break;
		case WAIT_PRESS:
			if (!input) {
				SOUND_STATE = WAIT;			
			}
			break;
		case PLAY:
			if (index >= totNotes) {
				if (input) {	// button is pressed and held at end
					SOUND_STATE = WAIT_PRESS;
				}
				else {
					SOUND_STATE = WAIT;
				}
			}
			break;
		default:
			SOUND_STATE = WAIT;
			index = 0;
			PWM_off();
			break;	
	}
	switch (SOUND_STATE) {
		case WAIT:
			PWM_off();
			index = 0;
			break;
		case WAIT_PRESS:
			PWM_off();
			index = 0;
			break;
		case PLAY:
			if (i >= (noteTime[index] / 50)) {
				PWM_off();
				while(!TimerFlag);
				TimerFlag = 0;
				index++;
				
				i = 0;
			}
			i++;
			if (!(index >= totNotes)) {
				PWM_on();
			}
			break;
	}
	return melody[index];
}

int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned char input = 0x00;
	double inFreq = 0x00;
	TimerSet(50);
	TimerOn();
	
	/* Insert your solution below */
	while (1) {
		input = ~PINA & 0x07;
		inFreq = PlaySound(input);
		set_PWM(inFreq);
		while (!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}

