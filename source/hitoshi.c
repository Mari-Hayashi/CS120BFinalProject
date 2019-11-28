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
#include "Nokia_5110.h"

#define reset (((~PINA) & 0x02) >> 1)
typedef enum boolean {true, false} boolean;
#pragma region  Timer Functions

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

#pragma endregion

#pragma region ADC
unsigned short ADCValue;
unsigned const short joyStickUpValue = 700;
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}
boolean joyStickUp() {
	ADCValue = ADC;
	return (ADCValue > joyStickUpValue) ? true : false;
}
#pragma endregion

#pragma region  Task things

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

task tasks[4];
const unsigned char tasksNum = 4;

#pragma endregion

#pragma region Game Manager

typedef enum GameState {Init, Start, Fail, Achieve} GameState;
boolean GameOver;
boolean GameClear;
boolean GameGoingOn;

int GameManager(int state) {
	if (reset) {
		GameState = Init;
		ResetEverything();
	}

	// Transition
	switch (state)
	{
		case Init:
			state = (joyStickUp() == true) ? Start : Init;
			break;
		case Start:
			if (GameOver == true)  state = Fail;
			else if (GameClear == true) state = Achieve;
			else state = Start;
		case Fail:
			state = (joyStickUp() == true) ? Init : Fail;
			break;
		case Achieve:
			state = (joyStickUp() == true) ? Init : Achieve;
			break;
		default:
			break;
	}

	// Action
	switch (state)
	{
		case Init:
			DisplayTitle();
			GameOver = false;
			GameClear = false;
			GameGoingOn = false;
			break;
		case Start:
			GameGoingOn = true;
		case Fail:
			GameGoingOn = false;
			DisplayGameOver();
			GameGoingOn = false;
			break;
		case Achieve:
			GameGoingOn = false;
			DisplayGameClear();
			GameGoingOn = false;
			break;
		default:
			break;
	}
	// Manage game
	return state;
}

void DisplayTitle() {
	nokia_lcd_set_cursor(0, 0);
	nokia_lcd_write_char('M', 3);
	nokia_lcd_write_char('u', 2);
	nokia_lcd_write_char('s', 2);
	nokia_lcd_write_char('i', 2);
	nokia_lcd_write_char('c', 2);

	nokia_lcd_set_cursor(10, 20);

	nokia_lcd_write_char('G', 3);
	nokia_lcd_write_char('a', 2);
	nokia_lcd_write_char('m', 2);
	nokia_lcd_write_char('e', 2);
}

void DisplayGameOver() {
	nokia_lcd_set_cursor(0, 0);
	nokia_lcd_write_char('G', 3);
	nokia_lcd_write_char('a', 2);
	nokia_lcd_write_char('m', 2);
	nokia_lcd_write_char('e', 2);

	nokia_lcd_set_cursor(10, 20);

	nokia_lcd_write_char('O', 3);
	nokia_lcd_write_char('v', 2);
	nokia_lcd_write_char('e', 2);
	nokia_lcd_write_char('r', 2);
}

void DisplayGameClear() {
	nokia_lcd_set_cursor(0, 0);
	nokia_lcd_write_char('G', 3);
	nokia_lcd_write_char('a', 2);
	nokia_lcd_write_char('m', 2);
	nokia_lcd_write_char('e', 2);

	nokia_lcd_set_cursor(10, 20);

	nokia_lcd_write_char('C', 3);
	nokia_lcd_write_char('l', 2);
	nokia_lcd_write_char('e', 2);
	nokia_lcd_write_char('a', 2);
	nokia_lcd_write_char('r', 2);
}
#pragma endregion

#pragma region Character Controller

typedef enum CharState
{
	ground, jump1, jump2, jump3, jump4, jump5
} CharState;
unsigned char jumpHeightArray[] = { 5, 10, 15, 10, 5 };
// Corresponds to jump1 - jump5
boolean CharacterIsJumping;

int Character(int state) {
	if (GameGoingOn == false) return;

	unsigned char height;
	// Transition
	switch (state)
	{
		case ground:
			state = (joyStickUp() == true) ? jump1 : ground;
			bdeak;
		case jump5:
			state = ground;
			break;
		default:
			state = state + 1;
			break;
	}

	// Action	
	switch (state)
	{
		case ground:
			CharacterIsJumping = false;
			height = 0;
			break;
		default:
			CharacterIsJumping = true;
			height = jumpHeightArray[curPos - 1];
			break;
	}

	nokia_lcd_set_cursor(10, 40 - height);
	nokia_lcd_write_char('X', 1);
	return state;
}

#pragma endregion

#pragma region Music Controller

const double frequencyArray[9] = { 0, 261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25 };
unsigned const char numFrequency = 9;

const unsigned char ABCSongFreqencies[97] = { 0,1,0,1,0,5,0,5,0,6,0,6,0,5,5,5,5,4,0,4,0,3,0,3,0,2,0,2,0,1,1,1,1,
	5,0,5,0,4,0,4,0,3,0,3,0,2,2,2,2,5,0,5,0,4,0,4,0,3,0,3,0,2,2,2,2,
	1,0,1,0,5,0,5,0,6,0,6,0,5,5,5,5,4,0,4,0,3,0,3,0,2,0,2,0,1,1,1,1 }; 
unsigned const char numNotes = 97;

int MusicPlay(int index) {
	if (GameGoingOn == false) return;
	if (index >= numNotes) GameClear = true;
	if (index < 0 || index >= numNotes) return;
	set_PWM(frequencyArray[ABCSongFreqencies[index]]);
	ProduceObstacles(index);

	return index += 1;
}

#pragma endregion

#pragma region  Obstacle Controller


const unsigned char ABCSongObstacles[97] = { 0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,
	1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,
	1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0 }
unsigned int Obstacles[] = { -1, -1, -1, -1, -1 };
unsigned const char ObstacleArraySize = 5;
unsigned char currentNumObstacles = 0;
unsigned char currentObstaclePosition = 0;

unsigned const char ObstacleInitialPosition = 80;
unsigned const char ObstacleSpeed = 5; // pixel
unsigned const char timeWaitForObstacle = 5; // times period ms

void ProduceObstacles(int index) {
	// Check if obstacle hits character
	if (!(index < 0 || index >= numNotes)) {
		if (ABCSongObstacles[index] == 1 && !CharacterIsJumping) {
			GameOver = true;
		}
	}

	// Produce Obstacles
	unsigned const ObstacleProductionIndex = index + timeWaitForObstacle;
	if (!(ObstacleProductionIndex < 0 || ObstacleProductionIndex >= numNotes)) {
		if (ABCSongObstacles[ObstacleProductionIndex] == 1) {

			unsigned char newIndex = currentObstaclePosition + currentNumObstacles;
			if (newIndex >= ObstacleArraySize) newIndex -= ObstacleArraySize;

			currentNumObstacles++;

			Obstacles[newIndex] = ObstacleInitialPosition;
		}
	}
}

int MoveObstacles(int state) {
	if (GameGoingOn == false) return;
	int i;
	for (i = 0; i < ObstacleArraySize; ++i) {
		Obstacles[i] = nokia_write_obstacle(Obstacles[i]);
	}
	return state;
}

int nokia_write_obstacle(int curPos) {
	if (curPos < 0) return curPos;

	nokia_lcd_set_cursor(curPos, 40);
	nokia_lcd_write_char('o', 1);
	curPos -= ObstacleSpeed;

	if (curPos < 0) {
		currentNumObstacles--;
		currentObstaclePosition++;
		if (currentObstaclePosition >= ObstacleArraySize) {
			currentObstaclePosition = 0;
		}
	}

	return curPos;
}

#pragma endregion

void ResetEverything() {

}

int main(void) {
	
    DDRA = 0x00; PORTA = 0xFF; // JoyStick: A0. Reset button: A1
    DDRB = 0xFF; PORTB = 0x00; // Nokia LCD
    DDRD = 0xFF; PORTD = 0x00; // LED output

    ADC_init();
	PWM_on();
    nokia_lcd_init();

	// Play Music and Produce Obstacle task
	unsigned char i = 0;
	tasks[i].period = 250;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = timeWaitForObstacle * -1;
	tasks[i].TickFct = &MusicPlay();

	// Player task
	i++;
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = ground;
	tasks[i].TickFct = &Character();

	// Produce obstacle task
	i++;
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = 0;
	tasks[i].TickFct = &MoveObstacles();

	// GameManager
	i++;
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = 0;
	tasks[i].TickFct = &GameManager();

	while (1){

		nokia_lcd_clear();

		for (i = 0; i < tasksNum; i++) {
			if (tasks[i].elapsedTime >= tasks[i].period) {
				tasks[i].state = tasks[i].TickFct(tasks[i].state);
				tasks[i].elapsedTime = 0;
			}
			tasks[i].elapsedTime += 1;
		}
		
		nokia_lcd_render();
		
		// TODO: Replace below with while (!TimerFlag); TimerFlag = 0;
		int s, h;
		for (s = 0; s < 30; ++s){
			for (h = 0; h < 3000; ++h);
		}
		
	}

    return 1;
}