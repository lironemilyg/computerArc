/*
 * Memory.c
 *
 *  Created on: 9 Dec 2017
 *      Author: lirongazit
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>


#define MEMORY_SIZE 4096
#define BUFF_SIZE 8

float mem[MEMORY_SIZE];

int importMemory(char* filename){
	FILE *fMem;
	int nRead = 0;
	unsigned int num;
    float f;
    int memIndex = 0;
    size_t buff_size = BUFF_SIZE;
	char* memLine = (char*)malloc(sizeof(char)*buff_size);
	char* hexStr = (char*)malloc(sizeof(char)*(buff_size+2));
	if(filename == NULL){  //check for valid filename
		return 0;
	}
	if(memLine == NULL){  //check if malloc success
		return 0;
	}
	fMem = fopen(filename, "r");
	if(fMem == NULL){ //check if file exist
		return 0;
	}
	while ((nRead = getline(&memLine, &buff_size, fMem)) != -1) {
		strncpy(hexStr,"0x",3);
		memLine[buff_size] = '\0';
		strcat(hexStr,memLine);
		sscanf(hexStr, "%x", &num);  // assuming you checked input
		f = *((float*)&num);
		mem[memIndex] = f;
		memIndex++;
	}
	free(hexStr);
	free(memLine);
	fclose(fMem);
	return 1;
}

int exportMemory(char* filename){
	FILE *fMem;
	int i = 0;
	if(filename == NULL){ //check for valid filename
		return 0;
	}
	fMem = fopen(filename, "w");
	if(fMem == NULL){ //check if file exist
			return 0;
	}
	for(i = 0; i< MEMORY_SIZE; i++){
		fprintf(fMem, "%08X\n", *(int*)&mem[i]);
	}
	fclose(fMem);
	return 1;
}

float getMemoryI(int i){
	return mem[i];
}

int getMemoryInstruction(int i){
	return *(unsigned int*)&mem[i];
}

void setMemoryI(int i, float val){
	mem[i] = val;
}

