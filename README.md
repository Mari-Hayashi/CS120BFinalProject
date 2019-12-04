## Description

This is a custom project for UCR CS/EE 120B, Introduction to Embedded Systems.

For my Custom Project, I designed and developed a rhythm action game using a Nokia LCD display, Joystick, buttons, and a speaker. A player can move a character (stick figure) on the Nokia LCD screen using the Joystick to avoid hitting into obstacles. The obstacles will appear according to the rhythm of the background music, so the player has to make the character jump by the rhythm.


## Technologies and Components
- SimAVR
- ATmega1284 Microcontroller
- Nokia 5100 LCD Screen
- Parallax 2-Axis Joystick
- Speaker
- Buttons

## User Guide

The goal of this game is to control the character to avoid obstacles. The player mainly uses a button and a joystick to play the game.

When the game starts, it displays the title of the game. The player can start the game by pressing the select button (connected to PC1)

Then it goes to song selection screen. The player can choose the song they want to play out of three songs. They can use the joystick to move the cursor between songs. They can select the song by pressing the select button (PC1).

Then it goes to difficulty selection screen. The player can choose the difficulty of the game. There are 4 difficulties: Easy, Mediam, Hard, and Endless. 
They can use the joystick to move the cursor between the difficulties. They can select the difficulty by pressing the select button (PC1).
In the Easy, Meriam, or Hard mode, the song plays only once. Harder the difficulty, the faster the song and the speed of the obstacle. The player wins the game when he or she did not hit into an obstacle until the end of the song.
In the Endless mode, the song repeats endlessly getting faster and faster. 

Then it goes to game screen. The circle obstacles scrolls from right to left. They appear by the rhythm of the background music. The player has to move the joystick upward to make the stickfigure on the left jump to avoid the obstacles. 

LOSE: When the character hits the obstacle, the song stops and it goes to the gameover screen. 
WIN: When he or she did not hit any obstacles until the end of the song, it goes to the game clear screen and the happy sound effect plays. (Except for the Endless mode. When the player completes the stage, the song plays again with faster speed.)
When the game ends, the screen displays the score, or the number of obstacles the player successfully avoided. The player can go back to the title screen by moving the joystick upward once.

Whenever the reset button (connected to PC0) is pressed, it goes back to the title screen.

## Library
Nokia 5110 LCD Library: github.com/pseudodan/CS120B_Embedded-Systems

## Demo Video on YouTube
https://youtu.be/9tfQdOKx-Co
