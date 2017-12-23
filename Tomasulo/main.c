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
	CPU c = initCPU(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
	runCPU(c);
	destroyCPU(c);
/*
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
		int res = setTag(regs[i],"sqwertyuiopasdfghjkl");
		int resGet=getTag(regs[i],&s);
		printf("Tag: %s valid: %d\n",s, isValid(regs[i]));
	}
	char *filenameout = "./regout333.txt";
	createRegout(filenameout,regs);

	char *filename = "./all_inst/memin.txt" ;
	char *filenameout = "./all_inst/memout2.txt" ;
	importMemory(filename);
	exportMemory(filenameout);


	ReservationStation r = initStation("dor");
	printf("isBusy BeforFill: %d\n", isBusy(r));
	printf("Fill Station Status: %d\n",fillStation(r, 0, 9, regs[0], regs[1], 0));
	printf("isBusy : %d\n", isBusy(r));
	printf("getIsReady : %d\n", getIsReady(r));
	printf("r->imm : %d\n", r->imm);
	//sschar* temp;
	//getRktag(r);
	printf("r->tag : %s\n", r->Rktag);*/
}
