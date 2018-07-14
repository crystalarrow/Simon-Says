/* 
 * Jonathan Ho (jho047@ucr.edu) 
 * 
 * Lab Section: 023
 * Assignment: Final Project, Simon Says
 *  
 * I acknowledge all content contained herein, excluding template or example
 *   code, is my own work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "io.c"
#include "timer.h"
#include "shift_register.c"
#include "led_matrix.c"
#include "custom_character.c"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned char gameStatus = 0; // game on or game off
unsigned char lose = 0; // player entered incorrect pattern or not
unsigned long pattern[255]; // stores the patterns
unsigned char level = 0; // index for patterns, increments by 1 every time the user correctly repeats the pattern
unsigned char userPattern[255]; // stores the user input
unsigned char userLoc = 0; // keeps track of user input patterns
unsigned char highScore;
unsigned char EEMEM eememory = 0;


/*********** Toggle Start ***********/
unsigned char button; // start button
enum Toggle_States {toggle_wait, toggle_buttonPress, toggle_buttonWait} toggle_state;
void toggle_tick(){
	switch(toggle_state){
		case toggle_wait:
			toggle_state = button ? toggle_buttonPress : toggle_wait;
			break;
		case toggle_buttonPress:
			toggle_state = toggle_buttonWait;
			break;
		case toggle_buttonWait:
			toggle_state = button ? toggle_buttonWait : toggle_wait;
			break;
		default:
			break;
	}
	switch(toggle_state){
		case toggle_wait:
			break;
		case toggle_buttonPress:
			level = 0;
			gameStatus = 1;
			nextPattern();
			delay_ms(500);
			break;
		case toggle_buttonWait:
			break;
		default:
			break;
	}
}
/*********** Toggle End ***********/

/*********** LCD Display Start ***********/
const unsigned char game[] = "   Simon Says   ";
const unsigned char msg[] = "Press the button to start";
unsigned char msgLen = sizeof(msg);
unsigned char count = 0;
enum LCD_States {lcd_wait, lcd_gameOff, lcd_gameOn, lcd_gameLost} lcd_state;
void lcd_tick(){
	unsigned char temp[msgLen];
	unsigned char test[0xFF];
	switch(lcd_state){ // transitions
		case lcd_wait:
			if(lose)
				lcd_state = lcd_gameLost;
			else
				lcd_state = gameStatus ? lcd_gameOn : lcd_gameOff;
			break;
		case lcd_gameOff:
			if(lose)
				lcd_state = lcd_gameLost;
			else
				lcd_state = gameStatus ? lcd_gameOn : lcd_gameOff;
			break;
		case lcd_gameOn:
			if(lose)
				lcd_state = lcd_gameLost;
			else
				lcd_state = gameStatus ? lcd_gameOn : lcd_gameOff;
			break;
		case lcd_gameLost:
			if(lose)
				lcd_state = lcd_gameLost;
			else
				lcd_state = gameStatus ? lcd_gameOn : lcd_gameOff;
			break;
		default:
			break;
	} 
	switch(lcd_state){ // actions
		case lcd_wait:
			break;
		case lcd_gameOff:
			LCD_ClearScreen();
			strncpy(temp, msg + count, msgLen);
			temp[msgLen] = '\0';
			if(count >= msgLen - 2)
				count = 0;
			strcat(test, game);
			strcat(test, temp);
			LCD_DisplayString(1, test);
			custom_char(smile, 1);
			custom_char(smile, 2);
			custom_char(smile, 15);
			custom_char(smile, 16);
			LCD_Cursor(34);
			count++;
			break;
		case lcd_gameOn:
			LCD_DisplayString(1, "Score:          High Score:");
			LCD_Cursor(7);
			if(level-1 == 0)
				LCD_WriteData(0 + '0');
			else
				printScore(level-1);
			LCD_Cursor(28);
			LCD_WriteData(highScore + '0');
			LCD_Cursor(34);
			break;
		case lcd_gameLost:
			LCD_ClearScreen();
			LCD_DisplayString(1, "    You Lose    High Score:");
			custom_char(frown, 1);
			custom_char(frown, 2);
			custom_char(frown, 15);
			custom_char(frown, 16);
			LCD_Cursor(28);
			LCD_WriteData(highScore + '0');
			LCD_Cursor(34);
			break;
		default:
			break;
	}
}

/*********** Display Pattern Start ***********/
unsigned char loc = 0;
enum Matrix_States {matrix_wait, matrix_one, matrix_two, matrix_three, matrix_four} matrix_state;
void matrix_tick(){
	switch(matrix_state){ // Transitions start
		case matrix_wait:
			if(pattern[loc - 1] == 1)
				matrix_state = matrix_one;
			else if(pattern[loc - 1] == 2)
				matrix_state = matrix_two;
			else if(pattern[loc - 1] == 3)
				matrix_state = matrix_three;
			else if(pattern[loc - 1] == 4)
				matrix_state = matrix_four;
			else
				matrix_state = matrix_wait;
			break;
		case matrix_one:
			matrix_state = matrix_wait;
			break;
		case matrix_two:
			matrix_state = matrix_wait;
			break;
		case matrix_three:
			matrix_state = matrix_wait;
			break;
		case matrix_four:
			matrix_state = matrix_wait;
			break;
		default:
			break;
	} // Transition end
	switch(matrix_state){ // Actions start
		case matrix_wait:
			zero();
			if(loc <= level)
				loc++;
			break;
		case matrix_one:
			one();
			break;
		case matrix_two:
			two();
			break;
		case matrix_three:
			three();
			break;
		case matrix_four:
			four();
			break;
		default:
			break;
	} // Actions end
}
/*********** Display Pattern End ***********/

/*********** IR Sensor Input Start ***********/
unsigned char ir1;
unsigned char ir2;
unsigned char ir3;
unsigned char ir4;

enum Input_States {input_wait, input_waitRelease, input_one, input_two, input_three, input_four} input_state;
void input_tick(){
	switch (input_state){ // Transitions start 
		case input_wait:
			if(ir1 && !(ir2 || ir3 || ir4)){
				input_state = input_one;
			}
			else if(ir2 && !(ir1 || ir3 || ir4)){
				input_state = input_two;
			}
			else if(ir3 && !(ir1 || ir2 || ir4)){
				input_state = input_three;
			}
			else if(ir4 && !(ir1 || ir2 || ir3)){
				input_state = input_four;
			}
			else{
				input_state = input_wait;
				if(userLoc == level)
					userLoc++;
			}
			break;
		case input_waitRelease:
			input_state = (ir1 || ir2 || ir3 || ir4) ? input_waitRelease : input_wait;
			break;
		case input_one:
			input_state = input_waitRelease;
			break;
		case input_two:
			input_state = input_waitRelease;
			break;
		case input_three:
			input_state = input_waitRelease;
			break;
		case input_four:
			input_state = input_waitRelease;
			break;
		default:   	   
			break;
	} // Transitions end 
	
	switch (input_state) { // Actions start 
		case input_wait:
			zero();
			break;
		case input_waitRelease:
			break;
		case input_one:
			one();
			userPattern[userLoc] = 1;
			userLoc++;
			break;
		case input_two:
			two();
			userPattern[userLoc] = 2;
			userLoc++;
			break;
		case input_three:
			three();
			userPattern[userLoc] = 3;
			userLoc++;
			break;
		case input_four:
			four();
			userPattern[userLoc] = 4;
			userLoc++;
			break;
		default:
			break;
	} // Actions end 
}
/*********** IR Sensor Input End ***********/


// Random Number generator (Generates numbers from 1 - 4)
int randNum(){
	return rand() % 4 + 1;
}

// Increases the difficulty of the level
void nextPattern(){
	pattern[level] = randNum();
	level++;
}

// checks the user input with the pattern
int check(){ 
	for(int i = 0; i < level; i++)
		if(pattern[i] != userPattern[i])
			return 0;
			
	return 1;
}

// Prints the score on lcd display
void printScore(int n) {
	if (n == 0) return; // need to check if the display is 0 before calling this function
	// Prints the score if it contains more than 1 digit
	printScore(n / 10);
	LCD_WriteData(n % 10 + '0');
}

int main()
{
	// Read high score from eeprom
	highScore = eeprom_read_byte(&eememory);
	
	// initialize random
	srand(time(NULL));
	
	/** Initialize all ports **/
	// PortA (input)
	DDRA = 0x00; PORTA = 0xFF; 
	// PortB (output)
	DDRB = 0xFF; PORTB = 0x00;
	// PortC (output)
	DDRC = 0xFF; PORTC = 0x00;
	// PortD (output)
	DDRD = 0xFF; PORTD = 0x00;  // LCD data lines

	/** Initialize state machines **/
	toggle_state = toggle_wait;
	lcd_state = lcd_wait;
	matrix_state = matrix_wait;
	input_state = input_wait;
	
	/** Initialize LCD Display **/
	LCD_init();
	LCD_ClearScreen();

	/** Initialize timer **/
	TimerSet(250);
	TimerOn();

    while(1) 
    {
    	/* Initialize IR sensors */
    	ir1 = ~PINA & 0x01;
    	ir2 = ~PINA & 0x02; 
    	ir3 = ~PINA & 0x04; 
    	ir4 = ~PINA & 0x08;

    	/* Initialize start button */  
    	button = ~PINA & 0x10;
		
		// Run lcd display
		lcd_tick();
		
    	/** Game Start **/
		if(lose){
			if(button){
				lose = 0;
				level = 0;
			}
		}
		if(!gameStatus && !lose){ // turns the game on
			all();
			toggle_tick();
		}
		if(gameStatus && !lose){ // game turned on, run game
			if(loc <= level) // display pattern
				matrix_tick();
			if(loc > level && userLoc <= level){ // wait for user input to repeat patter
				input_tick();
			}
			if(userLoc > level){ // check user answer
				if(check()){ // the pattern entered was correct, next level
					nextPattern();
					loc = 0;
					userLoc = 0;
					continue;
				}
				if(!check()){ // the pattern entered was wrong, end game
					all();
					if(level > highScore){ // checks if the current game score is higher than the high score, if it is update eeprom
						eeprom_update_byte(&eememory, level-1); // update eeprom to high score
						highScore = eeprom_read_byte(&eememory); // update highscore local tracker
					}
					lose = 1;
					loc = 0;
					userLoc = 0;
					gameStatus = 0;
				}
			}		
		}

		while(!TimerFlag){}
		TimerFlag = 0;
    }

	return 0;
}

