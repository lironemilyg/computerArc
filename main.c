/*
 * main.c
 *
 *  Created on: 9 Dec 2017
 *      Author: lirongazit
 */

#include "Register.c"
#include "Memory.c"

#define REGISTER_NUM 16
int main(int argc, char* argv[]){
	/*
	 * checking Register
	int i=0;
	float f;
	Register regs[REGISTER_NUM];
	for(i=0; i<REGISTER_NUM; i++){
		regs[i] = initRegister(i);
	}
	for(i=0; i<REGISTER_NUM; i++){
		int res = getValue(regs[i],&f);
		printf("val: %f valid: %d\n",f, res);
	}
	for(i=0; i<REGISTER_NUM; i++){
		char *s;
		int res = setTag(regs[i],"s");
		int resGet=getTag(regs[i],&s);
		printf("Tag: %s valid: %d\n",s, isValid(regs[i]));
	}
	*/

	/*
	 * Check Memory
	char *filename = "./all_inst/memin.txt" ;
	char *filenameout = "./all_inst/memout2.txt" ;
	importMemory(filename);
	exportMemory(filenameout);
	*/
}