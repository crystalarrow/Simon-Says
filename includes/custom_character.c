const char smile[8] = {0b00000,0b01010,0b01010,0b00000,0b00000,0b10001,0b01110,0b00000};
const char frown[8] = {0b01010, 0b01010, 0b01010, 0b01010, 0b00000, 0b01110, 0b10001, 0b00000};
const char box[8] = {0b11111,0b10001,0b10001,0b10001,0b10001,0b10001,0b10001,0b11111};

void custom_char(char *customCharacter, char loc){
	LCD_WriteCommand(0x40);
	for (int i = 0; i < 8; i++) {
		LCD_WriteData(customCharacter[i]);
	}
	LCD_Cursor(loc);
	LCD_WriteData(0);
}