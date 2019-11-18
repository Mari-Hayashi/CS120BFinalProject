#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

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

task tasks[1];
const unsigned char tasksNum = 1;

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
