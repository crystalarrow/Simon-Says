void zero(){ // all off
	PORTC = 0x00;
	transmit_data(0x00);
}

void one(){ // top left on
	PORTC = 0xF0;
	transmit_data(0x0F);
}

void two(){ // top right on
	PORTC = 0x0F;
	transmit_data(0x0F);
}

void three(){ // bottom left on
	PORTC = 0xF0;
	transmit_data(0xF0);
}

void four(){ // bottom right on
	PORTC = 0x0F;
	transmit_data(0xF0);
}

void all(){ // all on
	PORTC = 0xFF;
	transmit_data(0x00);
}

void display(int loc){ // function for debugging, tests each quadrant for display
	if(loc == 1){
		one();
	}
	else if(loc == 2){
		two();
	}
	else if(loc == 3){
		three();
	}
	else if(loc == 4){
		four();
	}
}