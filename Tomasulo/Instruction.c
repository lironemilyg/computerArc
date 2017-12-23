/*
 * Instruction.c
 *
 *  Created on: 16 Dec 2017
 *      Author: lirongazit
 */


typedef struct instruction{
	char* stationName;
	int instruction;
	int index, opcode, Ri, Rj, Rk, imm;
	float result;
	int issueCycle, startExCycle, endExCycle, writeCDBCycle;
}*Instruction;

void decode(Instruction i, int instruction){
	i->imm = instruction & 0xFFF; // IMM - 12 LSBs

	instruction = instruction >> 12;
	i->Rk = instruction & 0xF; // SRC1

	instruction = instruction >> 4;
	i->Rj = instruction & 0xF; // SRC0

	instruction = instruction >> 4;
	i->Ri = instruction & 0xF; // DST

	instruction = instruction >> 4;
	i->opcode = instruction & 0xF; // OPCODE
}

Instruction initInstruction(int index,int instruction){
	Instruction i = (Instruction)malloc(sizeof(Instruction*) + 1000);
	i->issueCycle = -1;
	i->startExCycle = -1;
	i->endExCycle = -1;
	i->writeCDBCycle = -1;
	i->index = index;
	decode(i, instruction);
	return i;
}

void destroyInstruction(Instruction i){
	free(i);
}

int getIndex(Instruction i){
	return i->index;
}

int getOpcode(Instruction i){
	return i->opcode;
}

int getRi(Instruction i){
	return i->Ri;
}

int getRj(Instruction i){
	return i->Rj;
}

int getRk(Instruction i){
	return i->Rk;
}

int getImm(Instruction i){
	return i->imm;
}

float getResult(Instruction i){
	return i->result;
}

void setResult(Instruction i, float result){
	i->result = result;
}

int getIssueCycle(Instruction i){
	return i->issueCycle;
}

void setIssueCycle(Instruction i, int cycle){
	i->issueCycle = cycle;
}

int getStartExCycle(Instruction i){
	return i->startExCycle;
}

void setStartExCycle(Instruction i, int cycle){
	i->startExCycle = cycle;
}

int getEndExCycle(Instruction i){
	return i->endExCycle;
}

void setEndExCycle(Instruction i, int cycle){
	i->endExCycle = cycle;
}

int getWriteCDBCycle(Instruction i){
	return i->writeCDBCycle;
}

void setWriteCDBCycle(Instruction i, int cycle){
	i->writeCDBCycle = cycle;
}

char* getStationName(Instruction i){
	return i->stationName;
}

void setStationName(Instruction i, char * name){
	i->stationName = name;
}


