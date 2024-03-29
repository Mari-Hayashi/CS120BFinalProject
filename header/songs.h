#include <avr/io.h>

const double frequencyArray[9] = {0, 261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25};

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

unsigned const char GameClearMusic[52] = {0,0,0,4,5,6,7,6,7,8,8,5,3,3,3,4,5,6,7,6,7,8,8,8,5,5,5,
	4,5,6,7,6,7,8,8,5,3,3,5,4,4,3,2,2,3,1,1,1,8,8,8,0};
unsigned const char ClearMusicLength = 52;
unsigned const char GameOverMusic[14] = {0,0,0,1,0,1,1,1,1,1,1,1,1,0};
unsigned const char FailMusicLength = 14;
