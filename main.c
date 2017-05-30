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
unsigned short I;
Uint8 *keys;
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
	int vx,vy,y,x,times,i;
	unsigned height,pixel;
	opcode = memory[pc] << 8 | memory[pc+1];
	//decode
	switch(opcode & 0xF000){
		case 0x00E0:

				memset(gfx,0,sizeof(gfx));
				break;
		case 0x2000:
				stack[sp] = pc;
				++sp;
				pc = opcode & 0x0FFF;
				break;
		case 0x1:
				pc = opcode & 0x0FFF;
				break;
		case 0x3:
				//skips if rX = NN
				if((registers[(opcode & 0x0F00) >> 8]) == (opcode &0x00FF) ){
					pc +=4;	

				} else{

					pc +=2;	
				}
				break;
		case 0x40:
				if((registers[(opcode & 0x0F00) >> 8]) != (opcode &0x00FF) ){
					pc +=4;	

				} else{

					pc +=2;	
				}
				break;
		case 0x5:
				if( registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4]){
					pc +=4;	

				} 
				else{
					pc +=2;
				}
				break;
	     case 0x6:
				//sets rX = NN
				registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
				pc+=2;
				break;
		 case 0x7:
				registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
				pc += 2;
				break;
		 case 0x8:
				switch(opcode & 0x000F){
					case 0x0000:
						registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
						pc +=2;
						break;
					case 0x0001:
						registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8]  | registers[(opcode & 0x00F0) >> 4];
						pc +=2;
						break;
					case 0x0002:
						registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8]  & registers[(opcode & 0x00F0) >> 4];
						pc +=2;
						break;
					
					case 0x0003:
						registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8]  ^ registers[(opcode & 0x00F0) >> 4];
						pc +=2;
						break;
					case 0x0004:
						if(((int)registers[(opcode & 0x0f00) >> 8] + (int)registers[(opcode & 0x00f00) >> 4]) < 256){
							registers[15] &= 0;

						} else{

							registers[15] =1;
						}
						registers[(opcode & 0x0f00) >> 8] +=registers[(opcode & 0x00f0) >> 4];
                        pc += 2;
						break;
					case 0x0005:
						if(((int)registers[(opcode & 0x0f00) >> 8] - (int)registers[(opcode & 0x00f00) >> 4]) < 256){
							registers[15] &= 0;

						} else{

							registers[15] =1;
						}
						registers[(opcode & 0x0f00) >> 8] -=registers[(opcode & 0x00f0) >> 4];
                        pc += 2;
						break;
					case 0x0006:
						registers[0xF] = registers[(opcode & 0x0F00) >> 8] & 7;
						registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] >> 1;
                        pc += 2;
						break;
					case 0x0007: // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
                        if(((int)registers[(opcode & 0x0F00) >> 8] - (int)registers[(opcode & 0x00F0) >> 4]) > 0)
                            registers[0xF] = 1;
                        else
                            registers[0xF] &= 0;

                        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4] - registers[(opcode & 0x0F00) >> 8];
                        pc += 2;
                    break;

                    case 0x000E: // 8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
                        registers[0xF] = registers[(opcode & 0x0F00) >> 8] >> 7;
                        registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] << 1;
                        pc += 2;
						break;

				}
				break;
				case 0x9000:
					if(registers[(opcode & 0x0F00) >> 8] != registers[(opcode & 0x00F0) >> 4]){
						pc +=4;

					}	else {
						pc +=2;

					}
					break;
				case 0xA000:
					 I = opcode & 0x0FFF;
					 pc +=2;
					 break;
				case 0xB000:
					 pc = (opcode & 0x0FFF) + registers[0];
					 break;
				case 0xC000:
					 registers[(opcode & 0x0F00) >> 8] = rand() & (opcode & 0x00FF);
					 pc +=2;
					 break;
				case 0xD000:
					 vx = registers[(opcode & 0x0F00) >> 8];
               		 vy = registers[(opcode & 0x00F0) >> 4];
                     height = opcode & 0x000F;  
               		 registers[0xF] &= 0;
           
                	for(y = 0; y < height; y++){
                    	pixel = memory[I + y];
                    	for(x = 0; x < 8; x++){
                        if(pixel & (0x80 >> x)){
                            if(gfx[x+vx+(y+vy)*64])
                                registers[0xF] = 1;
                            gfx[x+vx+(y+vy)*64] ^= 1;
                        }
                    }
                }
                pc += 2;
            break;
			case 0xE000:
                switch(opcode & 0x000F){
                    case 0x000E: // EX9E: Skips the next instruction if the key stored in VX is pressed
                        keys = SDL_GetKeyState(NULL);
                        if(keys[key[registers[(opcode & 0x0F00) >> 8]]])
                            pc += 4;
                        else
                            pc += 2;
                    break;
                             
                    case 0x0001: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
                        keys = SDL_GetKeyState(NULL);
                        if(!keys[key[registers[(opcode & 0x0F00) >> 8]]])
                            pc += 4;
                        else
                            pc += 2;
                    break;
                         
                }
            break;
					
				//now for some cancer
				



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

	for(;;){
		emulate();

	}





	return 0;

	}
