#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define NUMMEMORY 65536
#define NUMREG 8

typedef struct state_struct {//struct of state
	int pc;
	int num_memory;
	int mem[NUMMEMORY];
	int reg[NUMREG];
} statetype;

void print_state(statetype *stateptr){//prints out the state's data
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", stateptr->pc);
	printf("\tmemory:\n");
	for(i = 0; i < stateptr->num_memory; i++){
		printf("\t\tmem[%d]=%d\n", i, stateptr->mem[i]);
	}
	printf("\tregisters:\n");
	for(i = 0; i < 8; i++){
		printf("\t\treg[%d]=%d\n", i, stateptr->reg[i]);
	}
	printf("end state\n");
}

int convert_num(int num);//method to convert 16 bit negative numbers to 32 bit
int multiply(int mcand, int mplier);


int main(int argc, char** argv){
int c;
char *input = NULL;
int fileSize = 0;
char *buffer;
	while((c = getopt (argc, argv, "i:")) != -1)//takes in a file to read
	{
		switch(c)
		{
			case'i'://read in case
				input = optarg;
				break;
			case'?'://other case where no -i
			printf("ERROR: invalid option : valid option is -i program.txt\n");
			exit(-1);
		}//s
	}//W
	
FILE *file;
if(input == NULL)//null check on inputed file
{
	printf("ERROR: input is null\n");
	exit(-1);
}
file = fopen(input, "rb");// Taken from https://stackoverflow.com/questions/3747086/reading-the-whole-text-file-into-a-char-array-in-c/3747128
fseek(file, 0, SEEK_END);// On how to read a file in
fileSize = ftell(file); //gives file size
fseek(file, 0, SEEK_SET); 	
buffer = calloc( 1, fileSize+1 ); //allocates space for buffer to file size +1
if( !buffer ) {
	fclose(file),fputs("memory alloc fails",stderr),exit(1);
}
	
if( 1!=fread( buffer , fileSize, 1 , file) ){
	fclose(file),free(buffer),fputs("entire read fails",stderr),exit(1);
}

char *token;
int convertToken;
int lineCount = 1;

for(int i = 0; i < fileSize - 1; i++) { //counts new lines
	if(buffer[i] == 10){
		lineCount++;
	}
}


statetype* state  = (statetype*)malloc(sizeof(statetype));//creates struct variable
state->pc = 0;//sets pc to zero
state->num_memory = lineCount;//num_memory is the numbers if lines of code recieved

for(int i = 0; i < 8; i++)//intializes all of the registers to zero
{
	state->reg[i] = 0;
}//for

for(int i = 0; i < 65536; i++)//intializes all of the memory to zero
{
	state->mem[i] = 0;
}//for

const char s[1] = "\n";
	int count = 0;//memory position in which line will be place
	token = strtok(buffer , s); //seperates lines by \n
    		while( token != NULL ) {
        		convertToken = atoi(token);//converts line to int 
			state->mem[count] = convertToken;//places line into memory
			count++;//increase memory position that line will be place
        		token = strtok(NULL, s);
    		}

int regA = 0;
int regB = 0;
long int opcodeBit = 29360128;//2^24 + 2^23 + 2^22 = 0111 0000 0000 0000 0000 0000 0000
long int regABit = 3670016;//2^19 + 2^20 + 2^21 =         0011 1000 0000 0000 0000 0000
long int regBBit = 458752; //2^16 + 2^17 + 2^18 =              0111 0000 0000 0000 0000
long int offsetBit = 65535;// = 2^15 + 2^14 + ... + 2^0 =           1111 1111 1111 1111
int currentCounter;//holds current pc counter
long int currentInstruction;//holds the current opcode to do
long int tempLong;//holds the current line or code we are working with
int end = 0;//used to determine when to end
int instructions = 0 ;//hold number of instructions runs
while(end == 0)
{
	print_state(state);//print state
	currentCounter = state->pc;
	currentInstruction = state->mem[currentCounter];
	state->pc = state->pc+1;//increment pc so it fits jalr and others
	tempLong = currentInstruction & opcodeBit;
	tempLong = tempLong >> 22;
	if(tempLong == 6)//halt and end 
	{
		end = 1;//gets out of while loop
		printf("%s\n", "machine halted");
	}
	if(tempLong == 0)//add
	{
		long int tempA = currentInstruction & regABit;
		tempA = tempA >> 19;
		long int tempB = currentInstruction & regBBit;
		tempB = tempB >> 16;
		long int tempDest = currentInstruction & offsetBit;
		int total = state->reg[tempA] + state->reg[tempB];
		state-> reg[tempDest] = total;//sets the destination register to the sum of the two registers A and B
	}
	if(tempLong == 1)//nand
	{
		long int tempA = currentInstruction & regABit;
		tempA = tempA >> 19;
		long int tempB = currentInstruction & regBBit;
		tempB = tempB >> 16;
		long int tempDest = currentInstruction & offsetBit;
		int and = state->reg[tempA] & state->reg[tempB];
		and = ~and; 
		state-> reg[tempDest] = and;
	}
	if(tempLong == 2)//lw
	{
		long int tempA = currentInstruction & regABit;
		tempA = tempA >> 19;
		long int tempB = currentInstruction & regBBit;
		tempB = tempB >> 16;
		long int tempOff = currentInstruction & offsetBit;
		int tempRegB = state->reg[tempB];//grabs value from regB 
		int signNegative = currentInstruction & 32768;
		signNegative = signNegative >> 15;
		if(signNegative == 1)//checks if negative number 
		{
			tempOff = convert_num(tempOff);//if negative convert to 32 bit
		}

		if(state->pc+tempOff < 0 || state->pc+tempOff > 65536)
		{
			printf("ERROR: accessing outside of memory");
			exit(-1);
		}
		else
		{
			state->reg[tempA] = state->mem[tempRegB + tempOff];//set regA to the value at memory address of base of regB + incoming offset
		}
	}
	if(tempLong == 3)//sw
	{
		long int tempA = currentInstruction & regABit;
		tempA = tempA >> 19;
		long int tempB = currentInstruction & regBBit;
		tempB = tempB >> 16;
		long int tempOff = currentInstruction & offsetBit;
		int tempRegB = state->reg[tempB];
		int signNegative = currentInstruction & 32768;
		signNegative = signNegative >> 15;
		if(signNegative == 1)
		{
			tempOff = convert_num(tempOff);
		}
		if(state->pc+tempOff < 0 || state->pc+tempOff > 65536)
		{
			printf("ERROR: accessing outside of memory");
			exit(-1);
		}
		else
		{
			state->mem[tempRegB + tempOff] = state->reg[tempA];
		}
	}
	if(tempLong == 4)//beq
	{
		long int tempA = currentInstruction & regABit;
		tempA = tempA >> 19;
		long int tempB = currentInstruction & regBBit;
		tempB = tempB >> 16;
		long int tempOff = currentInstruction & offsetBit;
		int signNegative = currentInstruction & 32768;//32768 = 2^15 aka the bit that holds the sign for offset
		signNegative = signNegative >> 15;//this bit holds if negative or positive
		if(signNegative == 1)//if negative convert to 32 bit 
		{
			tempOff = convert_num(tempOff);
		}
		if(state->reg[tempA] == state->reg[tempB]){//if the two regs are equal then branch to new memory address 
			if(state->pc+tempOff < 0 || state->pc+tempOff > 65536)
			{
				printf("ERROR: accessing outside of memory");
				exit(-1);
			}
			else
			{
				state->pc = state->pc+tempOff; //pc + incoming offset is new location 
			}
		}

	}
	if(tempLong == 5)//jalr
	{
		long int tempA = currentInstruction & regABit;
		tempA = tempA >> 19;
		state->reg[tempA] = state->pc;//set regA to pc+1
		if(state->reg[tempB] < 0 || state->reg[tempB] > 65536)
		{
			printf("ERROR: accessing outside of memory");
			exit(-1);
		}
		long int tempB = currentInstruction & regBBit;
		tempB = tempB >> 16;
		state->pc = state->reg[tempB];//jump to value of regB
	}
	if(tempLong == 7)//noop and does nothing
	{

	}
	instructions = instructions +1;
	
}

}//main

int convert_num(int num)
{
	if (num & (1<<15) ) 
	{
		num -= (1<<16); // thats a minus equals
	}
	return(num);
}
