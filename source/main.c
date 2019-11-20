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
#include "nokia5110.h"

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

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
    unsigned char i;
	for (i=0; i < tasksNum; i++) {
	   if (tasks[i].elapsedTime >= tasks[i].period){
		  tasks[i].state = tasks[i].TickFct(tasks[i].state); 
		  tasks[i].elapsedTime = 0;
	   }
	   tasks[i].elapsedTime += 1;
	} 
}

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
	set_PWM(frequencyArray[songs[0].frequencies[index]]);
	index += 1;
	if (index >= songs[0].numNotes){
		index = 0;
	}
	return index;
}

int main(void) {
	
    DDRB = 0xFF; PORTB = 0x00; // Nokia LCD
    DDRD = 0xFF; PORTD = 0x00; // B output
    ADC_init();
    PWM_on();
    
    unsigned char i = 0;
	tasks[i].state = 0;
	tasks[i].period = 250;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &makeSound;
	
	songs[0].name = "ABC Song";
	songs[0].frequencies = {0,1,0,1,0,5,0,5,0,6,0,6,0,5,5,5,5,4,0,4,0,3,0,3,0,2,0,2,0,1,1,1,1,
		5,0,5,0,4,0,4,0,3,0,3,0,2,2,2,2,5,0,5,0,4,0,4,0,3,0,3,0,2,2,2,2,
		1,0,1,0,5,0,5,0,6,0,6,0,5,5,5,5,4,0,4,0,3,0,3,0,2,0,2,0,1,1,1,1};
	songs[0].numNotes = 97;
	
    //nokia_lcd_init();
    //nokia_lcd_clear();
    //nokia_lcd_set_cursor(0, 10);
    //nokia_lcd_write_char('k', 3);
    //nokia_lcd_render();
    
    TimerSet(1);
    TimerOn();
    
    int currentFrequency = 0;
    
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
