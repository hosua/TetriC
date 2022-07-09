#include "test.h"


// Stuff for testing shit
void init_test_1(){
	for (int y = 37; y <= FIELD_Y-1; y++){
		for (int x = 0; x < FIELD_X-1; x++){
			play_field[y][x] = 2;
		}
	}
}

void init_test_2(){
	for (int y = 30; y <= FIELD_Y-1; y++){
		if (y != 38)
			for (int x = 0; x < FIELD_X-1; x++){
				play_field[y][x] = 2;
			}
	}
	for (int x = 4; x <= 8; x++){
		play_field[38][x] = 3;
	}
}

void init_test_3(){
	for (int y = 34; y <= FIELD_Y-1; y++){
			for (int x = 0; x < FIELD_X-1; x++){
				play_field[y][x] = 2;
			}
	}
}

