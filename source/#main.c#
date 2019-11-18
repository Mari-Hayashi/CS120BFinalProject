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

#include "nokia5110.h"

unsigned short ADCValue;
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00; // B output
    DDRD = 0xFF; PORTD = 0x00; // D output
	//nokia_lcd_init();
	/*

	nokia_lcd_clear();
	nokia_lcd_write_string("Hello", 1);
	nokia_lcd_set_cursor(0, 10);
	nokia_lcd_write_string("World!", 1);
	nokia_lcd_render(); // Do I need this?
*/
	while (1){
		PORTD = 0xFF;
	}

    return 1;
}
