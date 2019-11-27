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

#include "PWM.h"
#include "Nokia_5110.h"
#include "songs.h"

typedef enum boolean {true, false} boolean;
typedef enum song { ABC, JingleBell, Flower } song;
typedef enum difficulty { EASY, MEDIUM, HARD, ENDLESS } difficulty;
unsigned const char numDifficulties = 4;

///////////////////// TASK ///////////////////////
typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

task tasks[8];
const unsigned char tasksNum = 8;

///////////////////// JOY STICK ///////////////////////
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

///////////////////// GAME MANAGER ///////////////////////
typedef enum GameState { Init, SongSelect, WaitButtonRelease, DifficultySelect, Start, Fail, 
	Achieve, WaitToBeReleased, Reset} GameState;
boolean GameOver;
boolean GameClear;
boolean GameGoingOn;
boolean songSelect;
boolean difficultySelect;
song Song;
difficulty Difficulty;
unsigned char numNotes;
unsigned char score;
unsigned short frequency;
unsigned const short defaultFrequency = 2000;

int GameManager(int state) {
	
	unsigned char selectButton;
	unsigned char resetButton;
	
	// Transition
	switch (state)
	{
		case Init:
			state = (joyStickUp() == true) ? SongSelect : Init;
			break;
		case SongSelect:
			selectButton = ((~PINC) & 0x02) >> 1;
			state = (selectButton) ? WaitButtonRelease : SongSelect;
			break;
		case WaitButtonRelease:
			selectButton = ((~PINC) & 0x02) >> 1;
			state = (selectButton) ? WaitButtonRelease : DifficultySelect;
			break;
		case DifficultySelect:
			selectButton = ((~PINC) & 0x02) >> 1;
			state = (selectButton) ? Start : DifficultySelect;
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
	
	resetButton = (~PINC) & 0x01;
	if (resetButton) state = Reset;
	
	// Action
	switch (state)
	{
		case Init:
			DisplayTitle();
			break;
		case SongSelect:
			songSelect = true;
			break;
		case DifficultySelect:
			songSelect = false;
			difficultySelect = true;
			break;
		case Start:
			difficultySelect = false;
			GameGoingOn = true;
			break;
		case Fail:
			GameGoingOn = false;
			DisplayGameOver();
			break;
		case Achieve:
			if (Difficulty == ENDLESS){
				frequency = frequency * 2 / 3;
				if (frequency < 1) frequency = 1;
				InitializeGame(true);
				state = Start;
			} else {
				GameGoingOn = false;
				DisplayGameClear();
			}
			break;
		case Reset:
			InitializeGame(false);
			DisplayTitle();
		default:
			break;
	}
	
	return state;
}

void DisplayTitle() {
	static char UpOrDown = 0; // 0 = up, 1 = down
	static const char maxHeight = 5;
	static char height = 0;
	
	if (UpOrDown == 0){
		height += 1;
		if (height >= maxHeight){
			UpOrDown = 1;
		}
	} else {
		height -= 1;
		if (height <= 0){
			UpOrDown = 0;
		}
	}
	
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
	
	// Draw music symbol
	
	unsigned char i;
	unsigned char j;
	
	for (i = 27; i < 37; ++i){
		nokia_lcd_set_pixel(72, i-height, 1);
		nokia_lcd_set_pixel(73, i-height, 1);
		nokia_lcd_set_pixel(74, i-height, 1);
	}
	
	for (i = 65; i < 75; ++i){
		nokia_lcd_set_pixel(i, 40-height, 1);
	}
	for (i = 66; i < 74; ++i){
		nokia_lcd_set_pixel(i, 39-height, 1);
		nokia_lcd_set_pixel(i, 41-height, 1);
	}
	for (i = 67; i < 73; ++i){
		nokia_lcd_set_pixel(i, 38-height, 1);
		nokia_lcd_set_pixel(i, 42-height, 1);
	}
	
	for (i = 0; i < 3; ++i){
		for (j = 75 + i; j < 80 + i; ++j){
			nokia_lcd_set_pixel(j, i + 27-height, 1);
		}
	}
}

void DisplayGameOver() {
	frequency = defaultFrequency;
	
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
	frequency = defaultFrequency;
	
	static char UpOrDown = 0; // 0 = up, 1 = down
	const char maxHeight = 3;
	static char height = 0;
	
	if (UpOrDown == 0){
		height += 1;
		if (height >= maxHeight){
			UpOrDown = 1;
		}
	} else {
		height -= 1;
		if (height <= 0){
			UpOrDown = 0;
		}
	}
	
	nokia_lcd_set_cursor(0, 0 + height);
	nokia_lcd_write_char('G', 3);
	nokia_lcd_write_char('a', 2);
	nokia_lcd_write_char('m', 2);
	nokia_lcd_write_char('e', 2);

	nokia_lcd_set_cursor(10, 20 + height);

	nokia_lcd_write_char('C', 3);
	nokia_lcd_write_char('l', 2);
	nokia_lcd_write_char('e', 2);
	nokia_lcd_write_char('a', 2);
	nokia_lcd_write_char('r', 2);
}

int PlaySoundEffect(int curPos){
	if (GameClear == true && curPos < ClearMusicLength){
		set_PWM(frequencyArray[GameClearMusic[curPos]]);
		return curPos + 1;
	} else if (GameOver == true && curPos < FailMusicLength){
		set_PWM(frequencyArray[GameOverMusic[curPos]]);
		return curPos + 1;
	} else {
		return curPos;
	}
}

///////////////////// SONG SELECT CONTROLLER ///////////////////////
typedef enum songSelectState { wait, getInput, waitRelease } songSelectState;
int ChooseSong(int state){
	if (songSelect == false) return wait;
	
	// Transition
	switch(state){
		case wait:
			state = (joyStickDown() == true || joyStickUp() == true) ? getInput : wait;
			break;
		case getInput:
			state = waitRelease;
			break;
		case waitRelease:
			state = (joyStickDown() == false && joyStickUp() == false) ? wait : waitRelease;
			break;
	}
	
	// Action
	if (state == getInput){
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

///////////////////// DIFFICULTY SELECT CONTROLLER ///////////////////////
int ChooseDifficulty(int state){
	if (difficultySelect == false) return wait;
	
	// Transition
	switch(state){
		case wait:
			state = (joyStickDown() == true || joyStickUp() == true) ? getInput : wait;
			break;
		case getInput:
			state = waitRelease;
			break;
		case waitRelease:
			state = (joyStickDown() == false && joyStickUp() == false) ? wait : waitRelease;
			break;
	}
	
	// Action
	if (state == getInput){
		if (joyStickDown() == true){
			Difficulty += 1;
			if (Difficulty >= numDifficulties) 
				Difficulty = numDifficulties - 1;
		}
		if (joyStickUp() == true){
			Difficulty -= 1;
			if (Difficulty < 0) Difficulty = 0;
		}
		
		switch (Difficulty){
			case EASY:
				frequency = 3000;
				break;
			case MEDIUM:
				frequency = 2000;
				break;
			case HARD:
				frequency = 1000;
				break;
			case ENDLESS:
				frequency = 3000;
				break;
		}
	}
	
	// Write
	nokia_lcd_set_cursor(10, 5);
	nokia_lcd_write_string("Easy", 1);
	nokia_lcd_set_cursor(10, 15);
	nokia_lcd_write_string("Medium", 1);
	nokia_lcd_set_cursor(10, 25);
	nokia_lcd_write_string("Hard", 1);
	nokia_lcd_set_cursor(10, 35);
	nokia_lcd_write_string("Endless", 1);
	
	nokia_lcd_set_cursor(0, (10 * Difficulty) + 5);
	nokia_lcd_write_char('o', 1);
	
	return state;
}

///////////////////// CHARACTER CONTROLLER ///////////////////////
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

///////////////////// SONG PLAY CONTROLLER ///////////////////////
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

///////////////////// OBSTACLES CONTROLLER ///////////////////////
signed char Obstacles[] = { -1, -1, -1, -1, -1 }; // Series of obstacles.
// Store the current position of obstacles. If negative, it's not on the screen.
unsigned const char ObstacleArraySize = 5; // Size of the array above.

unsigned const char ObstacleInitialPosition = 79; // x position.
unsigned const char ObstacleSpeed = 5; // pixel

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

///////////////////// SCORE DISPLAY ///////////////////////
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

///////////////////// GAME INITIALIZATION ///////////////////////
void InitializeGame(boolean keepData){
	
	GameOver = false;
	GameClear = false;
	GameGoingOn = false;
	songSelect = false;
	difficultySelect = false;
	
	if (keepData == false){
		score = 0;
		frequency = defaultFrequency;
		Song = ABC;
		Difficulty = EASY;
	}
	
	CharacterIsJumping = false;
	
	set_PWM(0);
	
	unsigned char j = 0;
	for (j = 0; j < ObstacleArraySize; ++j){
		Obstacles[j] = -1;
	}
	
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
	
	i++;
	
	// Sound effect
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = 0;
	tasks[i].TickFct = &PlaySoundEffect;
	
	i++;
	
	// Difficulty effect
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].state = wait;
	tasks[i].TickFct = &ChooseDifficulty;
}

///////////////////// MAIN ///////////////////////
int main(void) {
	
    DDRA = 0x00; PORTA = 0xFF; // Joystick (Input)
    DDRB = 0xFF; PORTB = 0x00; // Nokia LCD (Output)
    DDRC = 0x00; PORTC = 0xFF; // Buttons (Input)
    DDRD = 0xFF; PORTD = 0x00; // Nokia LCD (Output)
    
    ADC_init();
	PWM_on();
    nokia_lcd_init();
    
    InitializeGame(false);
    
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
			for (h = 0; h < frequency; ++h);
		}
		
	}
    return 1;
}
