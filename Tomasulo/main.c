/*
 * main.c
 *
 *  Created on: 9 Dec 2017
 *      Author: lirongazit
 */

#include "CPU.c"

int main(int argc, char* argv[]){
	if(argc != 7){
		return -1;
	}
	CPU c = initCPU(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]); //init CPU accorging args
	runCPU(c);//run CPU
	destroyCPU(c); //destroy and frr allocation
}
