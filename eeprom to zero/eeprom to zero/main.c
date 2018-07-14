/* 
 * Jonathan Ho (jho047@ucr.edu) 
 * 
 * Lab Section: 023
 * Assignment: Final Project, set eeprom to 0
 *  
 * I acknowledge all content contained herein, excluding template or example
 *   code, is my own work.
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include "io.c"

unsigned char EEMEM eememory = 0;

int main() {
	eeprom_update_byte(&eememory, 0);
	return 0;
}