/*	Author: mhaya010
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <util/delay.h>

#include "PWM.h"
#include "Timer.h"

unsigned short ADCValue;
unsigned const short joyStickUpValue = 700;
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

unsigned const char TimerPeriodGCD = 10;

int main(void) {
	
    DDRB = 0xFF; PORTB = 0x00; // Nokia LCD
    DDRD = 0xFF; PORTD = 0x00; // B output
    ADC_init();
    PWM_on();
    
	while (1){
		ADCValue = ADC;
		if( ADCValue > joyStickUpValue) {
			PORTD = 0x40;
		}
		else {
			PORTD = 0x00;
		}
	}
	
	//PWM_off();
    return 1;
}
