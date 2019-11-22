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

//#include "PWM.h"
#include "Nokia_5110.h"

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
    TCCR1B = 0x0B; // Timer controller register
    OCR1A = 125; // Output register
    TIMSK1 = 0x02; // Timer interrupt mask register
    TCNT1 = 0; // counter
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void TimerOff(){
    TCCR1B = 0x00;
}

ISR(TIMER1_COMPA_vect){
    _avr_timer_cntcurr --;
    if (_avr_timer_cntcurr == 0){
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M){
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

void TimerISR(){
	TimerFlag = 1;
}

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

typedef struct song {
  char* name;
  char numNotes;
  unsigned char* frequencies;
} song;

task tasks[1];
const unsigned char tasksNum = 1;

unsigned short ADCValue;
unsigned const short joyStickUpValue = 700;
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

unsigned const char TimerPeriodGCD = 10;
double frequencyArray[9] = {0, 261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25};
unsigned const char numFrequency = 9;

unsigned char ABCSong[97] = {0,1,0,1,0,5,0,5,0,6,0,6,0,5,5,5,5,4,0,4,0,3,0,3,0,2,0,2,0,1,1,1,1,
	5,0,5,0,4,0,4,0,3,0,3,0,2,2,2,2,5,0,5,0,4,0,4,0,3,0,3,0,2,2,2,2,
	1,0,1,0,5,0,5,0,6,0,6,0,5,5,5,5,4,0,4,0,3,0,3,0,2,0,2,0,1,1,1,1};
unsigned const char numNotes = 97;

unsigned char Notes[31] = {0,2,0,2,4,3,1,0,1,2,0,2,4,3,1,0,6,5,3,4,2,3,1,0,6,5,3,4,3,2,1};
unsigned char Timing[31] = {1,1,1,2,2,4,1,1,2,3,1,2,2,4,6,4,2,4,4,4,4,4,6,4,2,4,4,4,4,4,6};

song songs[1];

int makeSound(int index){
	//set_PWM(frequencyArray[songs[0].frequencies[index]]);
	index += 1;
	if (index >= songs[0].numNotes){
		index = 0;
	}
	return index;
}

void HelloWorld(){
	nokia_lcd_set_cursor(0, 0);
    nokia_lcd_write_char('H', 3);
    nokia_lcd_write_char('e', 2);
    nokia_lcd_write_char('l', 2);
    nokia_lcd_write_char('l', 2);
    nokia_lcd_write_char('o', 2);
    
    nokia_lcd_set_cursor(10, 20);
    
    nokia_lcd_write_char('W', 3);
    nokia_lcd_write_char('o', 2);
    nokia_lcd_write_char('r', 2);
    nokia_lcd_write_char('l', 2);
    nokia_lcd_write_char('d', 2);
    nokia_lcd_render();
}

unsigned const char speed = 5;

int ProduceObstacle(int curPos){
	if (curPos < 0) return curPos;
	nokia_lcd_clear();
	nokia_lcd_set_cursor(curPos, 40);
	nokia_lcd_write_char('o', 1);
	curPos -= speed;
	return curPos;
}

typedef enum CharState 
	{ground, jump1, jump2, jump3, jump4, jump5} CharState;
unsigned char jumpHeightArray[] = {5, 10, 15, 10, 5}; 
	// Corresponds to jump1 - jump5
	
int Character(int curPos){
	unsigned char height;
	
	if (curPos == ground){
		height = 0;
		if ((~PINA) != 0x00) curPos = jump1;
	} else {
		height = jumpHeightArray[curPos - 1];
		curPos = (curPos == jump5) ? ground : curPos + 1;
	}
	nokia_lcd_set_cursor(10, 40 - height);
	nokia_lcd_write_char('X', 1);
	return curPos;
}

int main(void) {
	
    DDRA = 0x00; PORTA = 0xFF; // Nokia LCD
    DDRB = 0xFF; PORTB = 0x00; // Nokia LCD
    DDRD = 0xFF; PORTD = 0x00; // LED output
    ADC_init();
    
    
    nokia_lcd_init();
	nokia_lcd_clear();
    
    unsigned char counter = 200;
    unsigned char curPos = 80;
    unsigned char CharacterState = ground;
    
	while (1){	
		
		curPos = ProduceObstacle(curPos);
		CharacterState = Character(CharacterState);
		
		nokia_lcd_render();
		
		int s, h;
		for (s = 0; s < 30; ++s){
			for (h = 0; h < 3000; ++h);
		}
		
		/*
		ADCValue = ADC;
		if( ADCValue > joyStickUpValue) {
			PORTD = 0x40;
		}
		else {
			PORTD = 0x00;
		}*/
	}
	
	//PWM_off();
    return 1;
}
