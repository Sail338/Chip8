#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#undef main
//current opcode
unsigned short opcode;
unsigned short ip;
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
unsigned char key[16] ={
   SDLK_0,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_5,
    SDLK_6,
    SDLK_7,
    SDLK_8,
    SDLK_9,
    SDLK_a,
    SDLK_b,
    SDLK_c,
    SDLK_d,
    SDLK_e,
    SDLK_f

	};

//i have no clue what this is but its our font set
unsigned char fontset[80] ={
 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
void init(char *path){
	pc = 0x200;
	sp = 0;
	memset(memory,0,4096);
	delay_timer = 0;
	sound_timer = 0;
	memset(registers,0,16*4);
	for(int i = 0;i<80;i++){
		memory[i] = fontset[i];

	}
	//opem the file and start placing it into to memory
	FILE* f = fopen(path,"rb");
	//find the size of the file
	fseek (f , 0 , SEEK_END);
	long fsize = ftell(f);
	rewind(f);
	char *buf = malloc(sizeof(char) * fsize +1);
	size_t result = fread(buf,1,fsize,f);
	//load into memory
	for(int i =0;i<result;i++){
		memory[i + 512] = buf[i];
		printf("%x\n",memory[i+512]);
	}
	fclose(f);

}
void emulate(){
	opcode = memory[pc] << 8 | memory[pc+1];
	//decode
	switch(opcode & 0xF000){
		case 0x00E0:
				memset(gfx,0,sizeof(gfx));
	}

}
int main(int argc,char ** argv){
	//setup SDL for grpahocs
    Uint8 * key;
	SDL_Window *window;
    SDL_Event event;
	//initalize the window
    SDL_Init(SDL_INIT_EVERYTHING);
	    window = SDL_CreateWindow(
        "An SDL2 window",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        640,                               // width, in pixels
        380,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );
		init(argv[1]);







	return 0;

	}
