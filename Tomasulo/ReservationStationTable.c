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
	ReservationStation* addStations;
	ReservationStation* mulStations;
	ReservationStation* divStations;
	ReservationStation* loadBuffer;
	ReservationStation* storeBuffer;
	int numOfAddStations;
	int numOfMulStations;
	int numOfDivStations;
	int numOfLoadBuffer;
	int numOfStoreBuffer;
	int addStationsInUse;
	int mulStationsInUse;
	int divStationsInUse;
	int loadBufferInUse;
	int storeBufferInUse;
}*ReservationStationTable;

ReservationStationTable initReservationStationTable(int add_nr_reservations, int mul_nr_reservations, int div_nr_reservations, int mem_nr_load_buffers, int mem_nr_store_buffers){
	int i = 0;
	int totalSize = add_nr_reservations + mul_nr_reservations + div_nr_reservations + mem_nr_load_buffers + mem_nr_store_buffers;
	ReservationStationTable rst = (ReservationStationTable)malloc(sizeof(ReservationStation)*totalSize + sizeof(int)*5);
	rst->addStationsInUse = 0;
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
	for(i = 0; i < add_nr_reservations; i++){
		char* name = "ADD_\0\0\0";
		sprintf(name,"ADD_%d",i);
		rst->addStations[i] = initStation(name);
	}
	rst->mulStations = (ReservationStation*)malloc(sizeof(ReservationStation**)*mul_nr_reservations);
	for(i = 0; i < mul_nr_reservations; i++){
		char* name = "MUL_\0\0\0";
		sprintf(name,"MUL_%d",i);
		rst->mulStations[i] = initStation(name);
	}
	rst->divStations = (ReservationStation*)malloc(sizeof(ReservationStation**)*div_nr_reservations);
	for(i = 0; i < div_nr_reservations; i++){
		char* name = "DIV_\0\0\0";
		sprintf(name,"DIV_%d",i);
		rst->divStations[i] = initStation(name);
	}
	rst->loadBuffer = (ReservationStation*)malloc(sizeof(ReservationStation**)*mem_nr_load_buffers);
	for(i = 0; i < mem_nr_load_buffers; i++){
		char* name = "LOAD_\0\0\0";
		sprintf(name,"LOAD_%d",i);
		rst->loadBuffer[i] = initStation(name);
	}
	rst->storeBuffer = (ReservationStation*)malloc(sizeof(ReservationStation**)*mem_nr_store_buffers);
	for(i = 0; i < mem_nr_store_buffers; i++){
		char* name = "STORE_\0\0\0";
		sprintf(name,"STORE_%d",i);
		rst->storeBuffer[i] = initStation(name);
	}
}

int readCDB(ReservationStationTable rst, Instruction i, int isHaltCPU = 0){
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
