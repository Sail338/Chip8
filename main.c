#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//current opcode
unsigned short opcode;
//our memory array
char memory[4096];

//registers we hjave 16
int registers[16];

int pc;
unsigned char gfx[64 * 32];
unsigned char delay_timer;
unsigned char sound_timer;
//some stack stuff
unsigned short stack[16];
unsigned short sp;


//emulate4s a keypad
unsigned char key[16];


