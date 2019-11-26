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
#include <avr/interrupt.h>

#include "PWM.h"
#include "Nokia_5110.h"
typedef enum boolean {true, false} boolean;
typedef enum song { ABC, JingleBell, Flower } song;
unsigned const char numSongs = 3;

unsigned const char ABCSongFrequencies[97] = {0,1,0,1,0,5,0,5,0,6,0,6,0,5,5,5,5,4,0,4,0,3,0,3,0,2,0,2,0,1,1,1,1,
	5,0,5,0,4,0,4,0,3,0,3,0,2,2,2,2,5,0,5,0,4,0,4,0,3,0,3,0,2,2,2,2,
	1,0,1,0,5,0,5,0,6,0,6,0,5,5,5,5,4,0,4,0,3,0,3,0,2,0,2,0,1,1,1,1};
const unsigned char ABCSongObstacles[97] = { 0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,
	1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,
	1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0 };
unsigned const char ABCnumNotes = 97;

unsigned const char JingleBellSongFrequencies[128] = {3,0,3,0,3,3,3,0,3,0,3,0,3,3,3,0,3,0,5,0,1,1,1,2,3,3,3,3,0,0,0,0,
	4,0,4,0,4,0,4,0,4,0,3,0,3,0,3,0,3,3,2,0,2,2,3,3,2,2,2,2,5,5,5,5,
	3,0,3,0,3,3,3,0,3,0,3,0,3,3,3,0,3,0,5,0,1,1,1,2,3,3,3,3,0,0,0,0,
	4,0,4,0,4,0,4,0,4,0,3,0,3,0,3,0,5,0,5,5,4,4,2,2,1,1,1,1,8,8,8,8};
unsigned const char JingleBellObstacles[128] =       {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0};
unsigned const char JBnumNotes = 128;

unsigned const char FlowerSongFrequencies[80] = {1,0,3,0,0,0,3,0,1,0,3,0,0,0,3,0,
	3,0,3,4,5,0,3,0,4,0,4,5,2,0,2,0,3,0,3,4,5,0,8,0,6,6,6,0,5,0,0,0,
	3,0,3,4,5,0,3,0,4,0,4,5,2,0,2,0,3,0,3,4,2,0,2,0,1,1,1,1,8,8,8,8};
unsigned const char FlowerSongObstacles[80] =   {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,
	1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0};
unsigned const char FlowerNumNotes = 80;

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

task tasks[6];
const unsigned char tasksNum = 6;

unsigned short ADCValue;
unsigned const short joyStickUpValue = 700;
unsigned const short joyStickDownValue = 300;
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}
boolean joyStickUp() {
	ADCValue = ADC;
	return (ADCValue > joyStickUpValue) ? true : false;
}
boolean joyStickDown() {
	ADCValue = ADC;
	return (ADCValue < joyStickDownValue) ? true : false;
}

double frequencyArray[9] = {0, 261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25};
unsigned const char numFrequency = 9;

typedef enum GameState { Init, SongSelect, Start, Fail, Achieve, WaitToBeReleased, Reset} GameState;
boolean GameOver;
boolean GameClear;
boolean GameGoingOn;
song Song;
unsigned const char GameClearMusic[51] = {0,0,0,4,5,6,7,6,7,8,8,5,3,3,3,4,5,6,7,6,7,8,8,8,5,5,5,
	4,5,6,7,6,7,8,8,5,3,3,5,4,4,3,2,2,3,1,1,1,8,8,8};
unsigned const char ClearMusicLength = 51;
unsigned const char GameOverMusic[14] = {0,0,0,1,0,1,1,1,1,1,1,1,1,0};
unsigned const char FailMusicLength = 14;
unsigned char curPosInSoundEffect;
boolean songSelect;
unsigned char numNotes;

int GameManager(int state) {
	
	unsigned char selectButton;
	
	// Transition
	switch (state)
	{
		case Init:
			state = (joyStickUp() == true) ? SongSelect : Init;
			break;
		case SongSelect:
			selectButton = ((~PINC) & 0x02) >> 1;
			state = (selectButton) ? Start : SongSelect;
			break;
		case Start:
			if (GameOver == true) state = Fail;
			else if (GameClear == true) state = Achieve;
			else state = Start;
			break;
		case Fail:
			state = (joyStickUp() == true) ? WaitToBeReleased : Fail;
			break;
		case Achieve:
			state = (joyStickUp() == true) ? WaitToBeReleased : Achieve;
			break;
		case WaitToBeReleased:
			state = (joyStickUp() == true) ? WaitToBeReleased : Reset;
			break;
		case Reset:
			state = Init;
			break;
		default:
			break;
	}
	
	unsigned char button = (~PINC) & 0x01;
	if (button) state = Reset;
	
	// Action
	switch (state)
	{
		case Init:
			DisplayTitle();
			break;
		case SongSelect:
			songSelect = true;
			break;
		case Start:
			songSelect = false;
			GameGoingOn = true;
			break;
		case Fail:
			GameGoingOn = false;
			curPosInSoundEffect = PlaySoundEffect(curPosInSoundEffect);
			DisplayGameOver();
			break;
		case Achieve:
			GameGoingOn = false;
			DisplayGameClear();
			curPosInSoundEffect = PlaySoundEffect(curPosInSoundEffect);
			break;
		case Reset:
			InitializeGame();
			DisplayTitle();
		default:
			break;
	}
	
	return state;
}

typedef enum songSelectState { wait, getInput, waitRelease } songSelectState;
int ChooseSong(int state){
	if (songSelect == false) return wait;
	
	// Transition
	switch(state){
		case wait:
			state = (joyStickDown() == true || joyStickUp() == true) ? getInput : wait;
			break;
		case getInput:
			PORTD = 0xFF;
			state = waitRelease;
			break;
		case waitRelease:
			state = (joyStickDown() == false && joyStickUp() == false) ? wait : waitRelease;
			break;
	}
	
	// Action
	if (state == getInput){
		PORTD = 0xFF;
		switch (Song){
			case ABC:
				Song = (joyStickDown() == true) ? JingleBell : ABC;
				break;
			case JingleBell:
				if (joyStickDown() == true) Song = Flower;
				else if (joyStickUp() == true) Song = ABC;
				else Song = JingleBell;
				break;
			case Flower:
				Song = (joyStickUp() == true) ? JingleBell : Flower;
				break;
		}
		
		switch (Song){
			case ABC:
				numNotes = ABCnumNotes;
				break;
			case JingleBell:
				numNotes = JBnumNotes;
				break;
			case Flower:
				numNotes = FlowerNumNotes;
				break;
		}
	}
	
	// Write
	nokia_lcd_set_cursor(10, 10);
	nokia_lcd_write_string("ABC Song", 1);
	nokia_lcd_set_cursor(10, 20);
	nokia_lcd_write_string("Jingle Bell", 1);
	nokia_lcd_set_cursor(10, 30);
	nokia_lcd_write_string("Flower Song", 1);
	
	nokia_lcd_set_cursor(0, 10 * (Song + 1));
	nokia_lcd_write_char('o', 1);
	
	return state;
}

int PlaySoundEffect(int curPos){
	if (GameClear == true && curPos < ClearMusicLength){
		set_PWM(frequencyArray[GameClearMusic[curPos]]);
		return curPos + 1;
	} else if (GameOver == true && curPos < FailMusicLength){
		set_PWM(frequencyArray[GameOverMusic[curPos]]);
		return curPos + 1;
	} else {
		set_PWM(0);
		return curPos;
	}
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

typedef enum CharState 
	{ground, jump1, jump2, jump3, jump4, jump5} CharState;
unsigned char jumpHeightArray[] = {5, 10, 15, 10, 5}; 
	// Corresponds to jump1 - jump5
boolean CharacterIsJumping;
	
int Character(int state){
	if (GameGoingOn == false) return ground;
	
	unsigned char height;
	
	// Transition
	switch (state)
	{
		case ground:
			state = (joyStickUp() == true) ? jump1 : ground;
			break;
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
			height = jumpHeightArray[state- 1];
			break;
	}

	nokia_lcd_set_cursor(10, 40 - height);
	nokia_lcd_write_char('X', 1);
	return state;
}

unsigned const char timeWaitForObstacle = 6; // times period ms.

int MusicPlay(int index) {
	if (GameGoingOn == false) {
		return timeWaitForObstacle * -1;
	}
	
	if (index >= numNotes) GameClear = true;
	
	ProduceObstacles(index);
	
	if (index < 0) return index + 1;
	else if (index >= numNotes) return index;
	
	unsigned char freqIndex;
	switch (Song){
		case ABC:
			freqIndex = ABCSongFrequencies[index];
			break;
		case JingleBell:
			freqIndex = JingleBellSongFrequencies[index];
			break;
		case Flower:
			freqIndex = FlowerSongFrequencies[index];
			break;
	}
	set_PWM(frequencyArray[freqIndex]);

	return index + 1;
}

signed char Obstacles[] = { -1, -1, -1, -1, -1 }; // Series of obstacles.
// Store the current position of obstacles. If negative, it's not on the screen.
unsigned const char ObstacleArraySize = 5; // Size of the array above.

unsigned const char ObstacleInitialPosition = 79; // x position.
unsigned const char ObstacleSpeed = 5; // pixel
unsigned char score = 0;

void ProduceObstacles(int index) {
	// Check if obstacle hits character
	signed char checkIndex = index - 1;
	if (!(checkIndex < 0 || checkIndex >= numNotes)) {
		if ((Song == ABC && ABCSongObstacles[checkIndex] == 1) ||
			(Song == JingleBell && JingleBellObstacles[checkIndex] == 1) ||
			(Song == Flower && FlowerSongObstacles[checkIndex] == 1))
		{
			if (CharacterIsJumping == true){
				score += 1;
			} else {
				GameOver = true;
			}
		}
	}

	// Produce Obstacles
	unsigned const ObstacleProductionIndex = index + timeWaitForObstacle;
	if (!(ObstacleProductionIndex < 0 || ObstacleProductionIndex >= numNotes)) {
		
		if ((Song == ABC && ABCSongObstacles[ObstacleProductionIndex] == 1) ||
			(Song == JingleBell && JingleBellObstacles[ObstacleProductionIndex] == 1) ||
			(Song == Flower && FlowerSongObstacles[ObstacleProductionIndex] == 1)) {
			int i = 0;
			for (i = 0; i < ObstacleArraySize; ++i){
				if (Obstacles[i] < 0){
					Obstacles[i] = ObstacleInitialPosition;
					break;
				}
			}
		}
	}
}

int MoveObstacles(int state) {
	if (GameGoingOn == false) return 0;
	int i;
	for (i = 0; i < ObstacleArraySize; ++i) {
		Obstacles[i] = nokia_write_obstacle(Obstacles[i]);
	}
	return 0;
}

int nokia_write_obstacle(int curPos) {
	if (curPos <= 0) return -1;

	nokia_lcd_set_cursor(curPos, 40);
	nokia_lcd_write_char('o', 1);

	return curPos - ObstacleSpeed;
}

int DisplayScore(int state){
	if (GameGoingOn == true){
		nokia_lcd_set_cursor(0,0);
		char str[] = "Score:"; 
		
		char val[5];
		itoa(score, val, 10);
		
		char *result = malloc(strlen(str) + strlen(val) + 1);
		strcpy(result, str);
		strcat(result, val);
		
		nokia_lcd_write_string(result, 1);
	} else if (GameOver == true || GameClear == true) {
		char str[] = "Score:"; 
		
		char val[5];
		itoa(score, val, 10);
		
		char *result = malloc(strlen(str) + strlen(val) + 1);
		strcpy(result, str);
		strcat(result, val);
		
		nokia_lcd_set_cursor(35,40);
		nokia_lcd_write_string(result, 1);
	}
	return 0;
}

void InitializeGame(){
	
	score = 0;
	
	GameOver = false;
	GameClear = false;
	GameGoingOn = false;
	songSelect = false;
	
	CharacterIsJumping = false;
	
	curPosInSoundEffect = 0;
	
	Song = ABC;
	
	set_PWM(0);
	
	//numNotes = ABCnumNotes;
	//strncpy(SongFrequencies, ABCSongFrequencies, numNotes);
	//strncpy(SongObstacles, ABCSongObstacles, numNotes);
	
	unsigned char i = 0;
	
	// Character controller
    tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = ground;
	tasks[i].TickFct = &Character;
	
	i ++;
	
	// Play Music and Produce Obstacle task
	tasks[i].period = 2;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = timeWaitForObstacle * -1;
	tasks[i].TickFct = &MusicPlay;
	
	i ++;
	
	// Move obstacle task
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = 0;
	tasks[i].TickFct = &MoveObstacles;
	
	i++;
	
	// Game manager
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = Init;
	tasks[i].TickFct = &GameManager;
	
	i++;
	
	// DisplayScore
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = Init;
	tasks[i].TickFct = &DisplayScore;
	
	i++;
	
	// Song select
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = wait;
	tasks[i].TickFct = &ChooseSong;
}

int main(void) {
	
    DDRA = 0x00; PORTA = 0xFF; // Joystick (Input)
    DDRB = 0xFF; PORTB = 0x00; // Nokia LCD (Output)
    DDRC = 0x00; PORTC = 0xFF; // Reset button (Input)
    DDRD = 0xFF; PORTD = 0x00; // LED output (Output for testing)
    
    ADC_init();
	PWM_on();
    nokia_lcd_init();
    
    InitializeGame();
    
	while (1){	
		
		nokia_lcd_clear();
		
		unsigned char i;
		for (i = 0; i < tasksNum; i++) {
			if (tasks[i].elapsedTime >= tasks[i].period) {
				tasks[i].state = tasks[i].TickFct(tasks[i].state);
				tasks[i].elapsedTime = 0;
			}
			tasks[i].elapsedTime += 1;
		}
		
		nokia_lcd_render();
		
		int s, h;
		for (s = 0; s < 30; ++s){
			for (h = 0; h < 3000; ++h);
		}
		
	}
    return 1;
}
