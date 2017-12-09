/*
 * main.c
 *
 *  Created on: 9 Dec 2017
 *      Author: lirongazit
 */


#include "Register.c"

int main(int argc, char* argv[]){
	int i=0;
	float f;
	Register regs[16];
	for(i=0; i<16; i++){
		regs[i] = initRegister(i);
	}
	for(i=0; i<16; i++){
		int res = getValue(regs[i],&f);
		printf("val: %f valid: %d",f, res);
	}
}
