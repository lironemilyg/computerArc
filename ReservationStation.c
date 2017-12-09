/*
 * ReservationStation.c
 *
 *  Created on: 9 Dec 2017
 *      Author: lirongazit
 */
#include <stdio.h>
#include "Register.c"

typedef struct station{
	int busy = 0;
	int ready = 0;
	int opcode = -1;
	char* Rjtag = NULL;
	char* Rktag = NULL;
	float RjVal;
	float RkVal;
}*ReservationStation;

ReservationStation initStation(){
	ReservationStation rs = (ReservationStation)malloc(sizeof(ReservationStation*));
	return rs;
}

int fillStation(ReservationStation r, int opcode, Register j, Register k){
	if(r->busy == 1){
		return -1;
	}
	r->busy = 1;
	r->opcode = opcode;
	if(isValid(j)){
		r->RjVal = getValue(j);
		if(r->Rjtag != NULL)
			free(r->Rjtag);
		r->Rjtag = NULL;
	}
	else{
		size_t size = sizeof(getTag(j));
		r->Rjtag =(char*)malloc(size);
		if(r->Rjtag == NULL)
			return 0;
		strncpy(r->Rjtag ,getTag(j), size);
	}
	if(isValid(k)){
		r->RkVal = getValue(k);
		if(r->Rktag != NULL)
			free(r->Rktag);
		r->Rktag = NULL;
	}
	else{
		size_t size = sizeof(getTag(k));
		r->Rktag =(char*)malloc(size);
		if(r->Rktag == NULL)
			return 0;
		strncpy(r->Rktag ,getTag(k), size);
	}
	if(r->Rjtag == NULL && r->Rktag == NULL){
		r->ready = 1;
	}
	else
		r->ready = 0;
	return 1;
}

int changeTag(ReservationStation r){
	return 0;
}
