/*
 * ReservationStationTable.c
 *
 *  Created on: 16 Dec 2017
 *      Author: lirongazit
 */

#include "ReservationStation.c"

#define LD 0
#define ST 1
#define ADD 2
#define SUB 3
#define MULT 4
#define DIV 5
#define HALT 6

typedef struct stations{
	ReservationStation* addStations;  //array of add RS
	ReservationStation* mulStations;  //array of mul RS
	ReservationStation* divStations;  //array of div RS
	ReservationStation* loadBuffer;   //array of load RS
	ReservationStation* storeBuffer;  //array of store RS
	int numOfAddStations; //total num of add RS
	int numOfMulStations; //total num of mul RS
	int numOfDivStations; //total num of div RS
	int numOfLoadBuffer;  //total num of load RS
	int numOfStoreBuffer; //total num of store RS
	int addStationsInUse; //total num of add RS in use
	int mulStationsInUse; //total num of mul RS in use
	int divStationsInUse; //total num of div RS in use
	int loadBufferInUse; //total num of load RS in use
	int storeBufferInUse; //total num of store RS in use
}*ReservationStationTable;

ReservationStationTable initReservationStationTable(int add_nr_reservations, int mul_nr_reservations, int div_nr_reservations, int mem_nr_load_buffers, int mem_nr_store_buffers){
	int i = 0; //init RS table
	int totalSize = add_nr_reservations + mul_nr_reservations + div_nr_reservations + mem_nr_load_buffers + mem_nr_store_buffers;
	ReservationStationTable rst = (ReservationStationTable)malloc(sizeof(ReservationStation)*totalSize + sizeof(int)*5);
	if(!rst){
		return NULL;
	}
	//printf("in initReservationStationTable- after malloc - DEBUG 2\n");
	//fflush(NULL);
	rst->addStationsInUse = 0; //there is no station in use
	rst->mulStationsInUse = 0;
	rst->divStationsInUse = 0;
	rst->loadBufferInUse = 0;
	rst->storeBufferInUse = 0;
	rst->numOfAddStations = add_nr_reservations;
	rst->numOfMulStations = mul_nr_reservations;
	rst->numOfDivStations = div_nr_reservations;
	rst->numOfLoadBuffer = mem_nr_load_buffers;
	rst->numOfStoreBuffer = mem_nr_store_buffers;
	rst->addStations = (ReservationStation*)malloc(sizeof(ReservationStation**)*add_nr_reservations);
	for(i = 0; i < add_nr_reservations; i++){ //init each RS
		char name[] = "ADD\0\0\0";
		sprintf(name,"ADD%d",i);
		rst->addStations[i] = initStation(name);
	}
	rst->mulStations = (ReservationStation*)malloc(sizeof(ReservationStation**)*mul_nr_reservations);
	for(i = 0; i < mul_nr_reservations; i++){//init each RS
		char name[] = "MUL\0\0\0";
		sprintf(name,"MUL%d",i);
		rst->mulStations[i] = initStation(name);
	}
	rst->divStations = (ReservationStation*)malloc(sizeof(ReservationStation**)*div_nr_reservations);
	for(i = 0; i < div_nr_reservations; i++){//init each RS
		char name[] = "DIV\0\0\0";
		sprintf(name,"DIV%d",i);
		rst->divStations[i] = initStation(name);
	}
	rst->loadBuffer = (ReservationStation*)malloc(sizeof(ReservationStation**)*mem_nr_load_buffers);
	for(i = 0; i < mem_nr_load_buffers; i++){//init each RS
		char name[] = "LOAD\0\0\0";
		sprintf(name,"LOAD%d",i);
		rst->loadBuffer[i] = initStation(name);
	}
	rst->storeBuffer = (ReservationStation*)malloc(sizeof(ReservationStation**)*mem_nr_store_buffers);
	for(i = 0; i < mem_nr_store_buffers; i++){//init each RS
		char name[] = "STORE\0\0\0";
		sprintf(name,"STORE%d",i);
		rst->storeBuffer[i] = initStation(name);
	}
	return rst;
}

int readCDB(ReservationStationTable rst, Instruction i, int isHaltCPU){ //update all station about read cdb
	int j = 0;
	for(j = 0; j < rst->numOfAddStations; j++){
		updateStation(rst->addStations[j],i);
	}
	for(j = 0; j < rst->numOfMulStations; j++){
		updateStation(rst->mulStations[j],i);
	}
	for(j = 0; j < rst->numOfDivStations; j++){
		updateStation(rst->divStations[j],i);
	}
	for(j = 0; j < rst->numOfLoadBuffer; j++){
		updateStation(rst->loadBuffer[j],i);
	}
	for(j = 0; j < rst->numOfStoreBuffer; j++){
		updateStation(rst->storeBuffer[j],i);
	}
	int opcode = getOpcode(i);
	switch(opcode){
		case LD:
			rst->loadBufferInUse--;
			break;
		case ST:
			rst->storeBufferInUse--;
			break;
		case ADD:
			rst->addStationsInUse--;
			break;
		case SUB:
			rst->addStationsInUse--;
			break;
		case MULT:
			rst->mulStationsInUse--;
			break;
		case DIV:
			rst->divStationsInUse--;
			break;
	}
	if (isHaltCPU) {
		// check if all stations are empty (all instructions have finished execution)
		if (rst->loadBufferInUse == 0 && rst->storeBufferInUse == 0 && rst->addStationsInUse == 0 && rst->mulStationsInUse == 0 && rst->divStationsInUse == 0) {
			return 1;
		}
	}
	return 0;
}

void insertInstruction(ReservationStationTable rst, int freeStationIndex, Instruction inst, Register* regs){ //insert inst to RS
	int opcode = getOpcode(inst);
	int imm = getImm(inst);
	int index = getIndex(inst);
	switch(opcode){
		case LD:
			fillStation(rst->loadBuffer[freeStationIndex], index, opcode, regs[getRj(inst)], regs[getRk(inst)], imm);
			rst->loadBufferInUse++;
			setStationName(inst,getResStationName(rst->loadBuffer[freeStationIndex]));
			break;
		case ST:
			fillStation(rst->storeBuffer[freeStationIndex], index, opcode, regs[getRj(inst)], regs[getRk(inst)], imm);
			rst->storeBufferInUse++;
			setStationName(inst,getResStationName(rst->storeBuffer[freeStationIndex]));
			break;
		case MULT:
			fillStation(rst->mulStations[freeStationIndex], index, opcode, regs[getRj(inst)], regs[getRk(inst)], imm);
			rst->mulStationsInUse++;
			setStationName(inst,getResStationName(rst->mulStations[freeStationIndex]));
			//printf("in insertInstruction- station %d %d %d %d %s- DEBUG 2\n",rst->mulStations[freeStationIndex]->index, rst->mulStations[0]->busy, rst->mulStations[0]->ready, rst->mulStations[0]->opcode, rst->mulStations[0]->name);
			//		fflush(NULL);
			break;
		case DIV:
			fillStation(rst->divStations[freeStationIndex], index, opcode, regs[getRj(inst)], regs[getRk(inst)], imm);
			rst->divStationsInUse++;
			setStationName(inst,getResStationName(rst->divStations[freeStationIndex]));
			break;
		case ADD:
			fillStation(rst->addStations[freeStationIndex], index, opcode, regs[getRj(inst)], regs[getRk(inst)], imm);
			rst->addStationsInUse++;
			setStationName(inst,getResStationName(rst->addStations[freeStationIndex]));
			//printf("in insertInstruction- station %d %s %s- DEBUG 2\n",rst->addStations[freeStationIndex]->index, rst->addStations[freeStationIndex]->Rjtag, rst->addStations[freeStationIndex]->Rktag);
			//fflush(NULL);
			break;
		case SUB:
			fillStation(rst->addStations[freeStationIndex], index, opcode, regs[getRj(inst)], regs[getRk(inst)], imm);
			rst->addStationsInUse++;
			setStationName(inst,getResStationName(rst->addStations[freeStationIndex]));
			break;
	}

	if(opcode != ST){
		setTag(regs[getRi(inst)],getStationName(inst));
	}
}

int isStationFree(ReservationStationTable rst, int opcode){ //check if there is free station according OC
	int i;
	switch(opcode){
		case LD:
			for(i = 0; i < rst->numOfLoadBuffer; i++){
				if(!isBusy(rst->loadBuffer[i])){
					return i;
				}
			}
			break;
		case ST:
			for(i = 0; i < rst->numOfStoreBuffer; i++){
				if(!isBusy(rst->storeBuffer[i])){
					return i;
				}
			}
			break;
		case ADD:
			for(i = 0; i < rst->numOfAddStations; i++){
				if(!isBusy(rst->addStations[i])){
					return i;
				}
			}
			break;
		case SUB:
			for(i = 0; i < rst->numOfAddStations; i++){
				if(!isBusy(rst->addStations[i])){
					return i;
				}
			}
			break;
		case MULT:
			for(i = 0; i < rst->numOfMulStations; i++){
				if(!isBusy(rst->mulStations[i])){
					return i;
				}
			}
			break;
		case DIV:
			for(i = 0; i < rst->numOfDivStations; i++){
				if(!isBusy(rst->divStations[i])){
					return i;
				}
			}
			break;
	}
	return -1;
}

void destroyReservationStationTable(ReservationStationTable rst){ //destroy and free allocation
	if(rst != NULL){
		int j = 0;
		for(j = 0; j < rst->numOfAddStations; j++){
			destroyReservationStation(rst->addStations[j]);
		}
		for(j = 0; j < rst->numOfMulStations; j++){
			destroyReservationStation(rst->mulStations[j]);
		}
		for(j = 0; j < rst->numOfDivStations; j++){
			destroyReservationStation(rst->divStations[j]);
		}
		for(j = 0; j < rst->numOfLoadBuffer; j++){
			destroyReservationStation(rst->loadBuffer[j]);
		}
		for(j = 0; j < rst->numOfStoreBuffer; j++){
			destroyReservationStation(rst->storeBuffer[j]);
		}
		free(rst->addStations);
		free(rst->mulStations);
		free(rst->divStations);
		free(rst->loadBuffer);
		free(rst->storeBuffer);
		free(rst);
	}
}
