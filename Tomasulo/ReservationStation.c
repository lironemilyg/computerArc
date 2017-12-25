/*
 * ReservationStation.c
 *
 *  Created on: 9 Dec 2017
 *      Author: lirongazit
 */
#include <stdio.h>
#include <string.h>
#include "Register.c"
#include "InstructionQueue.c"

#define MAX_STRING 1000;
typedef struct station{
	char* name;
	int index;
	int busy;
	int ready;
	int opcode;
	char* Rjtag;
	char* Rktag;
	int RjTagValid;
	int RkTagValid;
	float RjVal;
	float RkVal;
	int inExecution;
	int imm;
}*ReservationStation;

ReservationStation initStation(char* stationName){
	int size = MAX_STRING;
	ReservationStation rs = (ReservationStation)malloc(sizeof(ReservationStation*) + size);
	rs->name = (char*)malloc(sizeof(stationName));
	strcpy(rs->name,stationName);
	rs->busy = 0;
	rs->ready = 0;
	rs->opcode = -1;
	rs->Rjtag = "";
	rs->Rktag = "";
	rs->RjTagValid = 0;
	rs->RkTagValid = 0;
	return rs;
}

char* getResStationName(ReservationStation r){
	return r->name;
}

float getValuej(ReservationStation r){
	return r->RjVal;
}

float getValuek(ReservationStation r){
	return r->RkVal;
}

int isBusy(ReservationStation r){
	if(r == NULL)
		return -1;
	return r->busy;
}

int getInExecution(ReservationStation r){
	if(r == NULL)
		return -1;
	return r->inExecution;
}

int setInExecution(ReservationStation r){
	if(r == NULL)
		return -1;
	r->ready = 0;
	r->inExecution = 1;
	return 1;
}

void changeTagj(ReservationStation r, float RjVal){
	r->RjVal = RjVal;
	free(r->Rjtag);
	r->RjTagValid = 0;
	//r->Rjtag = NULL;
}

void changeTagk(ReservationStation r, float RkVal){
	r->RkVal = RkVal;
	free(r->Rktag);
	r->RkTagValid = 0;
	//r->Rktag = NULL;
}

int getIsReady(ReservationStation r){
	return r->ready;
}

char* getRjtag(ReservationStation r){
	return r->Rjtag;
}

char* getRktag(ReservationStation r){
	return r->Rktag;
}

void setIsReady(ReservationStation r){
	if(r->Rjtag == NULL && r->Rktag == NULL)
		r->ready = 1;
	else
		r->ready = 0;
}

void emptyStation(ReservationStation r){
	r->busy = 0;
	r->RjTagValid = 0;
	r->RkTagValid = 0;
}

void updateStation(ReservationStation r, Instruction i){
	if(strcmp(r->Rjtag,i->stationName) == 0)
		changeTagj(r,getResult(i));
	if(strcmp(r->Rktag,i->stationName) == 0)
		changeTagk(r,getResult(i));
	setIsReady(r);
	if(strcmp(r->name,i->stationName) == 0)
		emptyStation(r);
}

int fillStation(ReservationStation r, int opIndex, int opcode, Register j, Register k, int imm){
	if(!r || r->busy == 1){
		return -1;
	}
	r->busy = 1;
	r->index = opIndex;
	r->inExecution = 0;
	r->opcode = opcode;
	r->imm = imm;
	if(isValid(j)){
		getValue(j,&r->RjVal);
		if(r->RjTagValid != 0)
			free(r->Rjtag);
		r->RjTagValid = 0;
	}
	else{
		char* tempTag;
		size_t size;
		getTag(j,&tempTag);
		size = strlen(tempTag);
		r->Rjtag =(char*)malloc(size + 1);
		if(r->Rjtag == NULL)
			return 0;
		strcpy(r->Rjtag ,tempTag);
		r->RjTagValid = 1;
	}
	if(isValid(k)){
		getValue(k,&r->RkVal);
		if(r->RkTagValid != 0)
			free(r->Rktag);
		r->RkTagValid = 0;
	}
	else{
		char* tempTag;
		size_t size;
		getTag(k,&tempTag);
		size = strlen(tempTag);
		r->Rktag =(char*)malloc(size + 1);
		if(r->Rktag == NULL)
			return 0;
		strcpy(r->Rktag ,tempTag);
		r->RkTagValid = 1;
	}
	setIsReady(r);
	return 1;
}

int getIndexFromRsStation(ReservationStation r){
	return r->index;
}

void destroyReservationStation(ReservationStation r){
	if(r != NULL){
		if(r->RjTagValid)
			free(r->Rjtag);
		if(r->RkTagValid)
			free(r->Rktag);
		free(r->name);
		free(r);
	}
}
