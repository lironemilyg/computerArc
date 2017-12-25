/*
 * CPU.c
 *
 *  Created on: 16 Dec 2017
 *      Author: lirongazit
 */

#include "Memory.c"
#include "ReservationStationTable.c"
#include <ctype.h>

#define REGISTER_NUM 16
#define LD 0
#define ST 1
#define ADD 2
#define SUB 3
#define MULT 4
#define DIV 5
#define HALT 6
#define CONFIG_LINE_SIZE 1024

typedef struct cpu{
	int cycle;
	int done;
	int halt;
	int add_nr_units;
	int mul_nr_units;
	int div_nr_units;
	int add_nr_reservations;
	int mul_nr_reservations;
	int div_nr_reservations;
	int add_delay;
	int mul_delay;
	int div_delay;
	int mem_delay;
	int mem_nr_load_buffers;
	int mem_nr_store_buffers;
	int add_in_use;
	int mul_in_use;
	int div_in_use;
	int mem_in_use;
	int write_cdb_add;
	int write_cdb_mul;
	int write_cdb_div;
	int write_cdb_mem;
	char *cfg, *memin, *memout, *regout, *traceInst, *traceCDB;
	Register regs[REGISTER_NUM];
	ReservationStationTable stations;
	InstructionQueue queue;
}*CPU;

int max(int num1, int num2) {

   /* local variable declaration */
   int result;

   if (num1 > num2)
      result = num1;
   else
      result = num2;

   return result;
}

void trim(char* string){
    int dest;
    int src=0;
    int len = strlen(string);

    // Advance src to the first non-whitespace character.
    while(isspace(string[src])) src++;

    // Copy the string to the "front" of the buffer
    for(dest=0; src<len; dest++, src++)
    {
        string[dest] = string[src];
    }

    // Working backwards, set all trailing spaces to NULL.
    for(dest=len-1; isspace(string[dest]); --dest)
    {
    	string[dest] = '\0';
    }
}

char* split(char* line, char delimiter){
	char* start = strchr(line,delimiter);
	start++;
	trim(start);
	return start;
}

int configCPU(CPU c){
	FILE *fCfp;
	int nRead = 0;
	size_t buff_size = CONFIG_LINE_SIZE;
	char* line = (char*)malloc(sizeof(char)*CONFIG_LINE_SIZE);
	if(c->cfg == NULL){ //check for valid filename
		return 0;
	}
	fCfp = fopen(c->cfg, "r");
	if(fCfp == NULL){ //check if file exist
		return 0;
	}
	nRead = getline(&line, &buff_size, fCfp);
	c->add_nr_units = atoi(split(line,'='));
	nRead = getline(&line, &buff_size, fCfp);
	c->mul_nr_units = atoi(split(line,'='));
	nRead = getline(&line, &buff_size, fCfp);
	c->div_nr_units = atoi(split(line,'='));
	nRead = getline(&line, &buff_size, fCfp);
	c->add_nr_reservations = atoi(split(line,'='));
	nRead = getline(&line, &buff_size, fCfp);
	c->mul_nr_reservations = atoi(split(line,'='));
	nRead = getline(&line, &buff_size, fCfp);
	c->div_nr_reservations = atoi(split(line,'='));
	nRead = getline(&line, &buff_size, fCfp);
	c->add_delay = atoi(split(line,'='));
	nRead = getline(&line, &buff_size, fCfp);
	c->mul_delay = atoi(split(line,'='));
	nRead = getline(&line, &buff_size, fCfp);
	c->div_delay = atoi(split(line,'='));
	nRead = getline(&line, &buff_size, fCfp);
	c->mem_delay = atoi(split(line,'='));
	nRead = getline(&line, &buff_size, fCfp);
	c->mem_nr_load_buffers = atoi(split(line,'='));
	nRead = getline(&line, &buff_size, fCfp);
	c->mem_nr_store_buffers = atoi(split(line,'='));
	free(line);
	fclose(fCfp);
	return 1;
}

CPU initCPU(char* cfg, char* memin, char* memout, char* regout, char* traceInst, char* traceCDB){
	int i;
	//ReservationStationTable stations;
	InstructionQueue queue;
	CPU c = (CPU)malloc(sizeof(CPU*)+sizeof(char)*300);
	if(c == NULL){
		return c;
	}
	for(i=0; i < REGISTER_NUM; i++){
		c->regs[i] = initRegister(i);
	}
	for(i=0; i<REGISTER_NUM; i++){
		float f;
		int res = getValue(c->regs[i],&f);
		printf("val: %f valid: %d\n",f, res);
	}
	c->cycle = 0;
	c->add_in_use = 0;
	c->mul_in_use = 0;
	c->div_in_use = 0;
	c->mem_in_use = 0;
	c->done = 0;
	c->halt = 0;
	c->cfg = cfg;
	c->memin = memin;
	c->memout = memout;
	c->regout = regout;
	c->traceInst = traceInst;
	c->traceCDB = traceCDB;
	c->write_cdb_add = 0;
	c->write_cdb_mul = 0;
	c->write_cdb_div =0;
	c->write_cdb_mem = 0;

	if(configCPU(c) == 0){
		free(c);
		return NULL;
	}
	ReservationStationTable stations = initReservationStationTable(c->add_nr_reservations, c->mul_nr_reservations, c->div_nr_reservations, c->mem_nr_load_buffers, c->mem_nr_store_buffers);
	queue = initInstructionQueue();
	c = (CPU)realloc(c,sizeof(*c) + sizeof(stations) + sizeof(queue) + 24);
	c->stations = stations;
	c->queue = queue;
	return c;
}

void writeToTraceCDB(FILE *fTraceCDB, InstructionNode in, int cycle, char * CDBName){
	fprintf(fTraceCDB, "%d %d %s %.5f %s\n", cycle, in->inst->index, CDBName, getResult(in->inst), getStationName(in->inst));
}

void writeCDB(CPU c){
	FILE *fTraceCDB;
	printf("in enter -  DEBUG\n");
	fflush(NULL);
	if(c->traceCDB == NULL){ //check for valid filename
		return;
	}
	fTraceCDB = fopen(c->regout, "a");
	if(fTraceCDB == NULL){ //check if file exist
			return;
	}
	InstructionNode in = c->queue->issueInstsTail;
	int opcode;
	char* tag;
	int cnt = 0;
	printf("getWriteCDBCycle(in->inst): %d \n", getWriteCDBCycle(in->inst));
	fflush(NULL);
	while(in != NULL && cnt < 5){
		if(getWriteCDBCycle(in->inst) == c->cycle){
			cnt++;
			c->done = readCDB(c->stations, in->inst, c->halt);
			getTag(c->regs[getRi(in->inst)], &tag);
			printf("in WriteCDB is valid: %d tag: %s stationName: %s - DEBUG\n", isValid(c->regs[getRi(in->inst)]), tag, getStationName(in->inst));
			fflush(NULL);
			if((!isValid(c->regs[getRi(in->inst)])) && strcmp(tag, getStationName(in->inst)) == 0){
				printf("**DEBUG** in WriteCDB- setting up Value of reg\n");
				fflush(NULL);
				setValue(c->regs[getRi(in->inst)], getResult(in->inst));
			}
			opcode = getOpcode(in->inst);
			switch(opcode){
				case LD:
					writeToTraceCDB(fTraceCDB, in, c->cycle, "MEM");
					c->mem_in_use--;
					break;
				case ST:
					writeToTraceCDB(fTraceCDB, in, c->cycle, "MEM");
					setMemoryI(getImm(in->inst), getResult(in->inst));
					c->mem_in_use--;
					break;
				case ADD:
					writeToTraceCDB(fTraceCDB, in, c->cycle, "ADD");
					c->add_in_use--;
					break;
				case SUB:
					writeToTraceCDB(fTraceCDB, in, c->cycle, "ADD");
					c->add_in_use--;
					break;
				case MULT:
					writeToTraceCDB(fTraceCDB, in, c->cycle, "MUL");
					c->mul_in_use--;
					break;
				case DIV:
					writeToTraceCDB(fTraceCDB, in, c->cycle, "DIV");
					c->div_in_use--;
					break;
			}
		}
		in = in->prev;
	}
	fclose(fTraceCDB);
}

void execute(CPU c, Instruction i,float Vj, float Vk){
	setStartExCycle(i,c->cycle);
	switch(i->opcode){
		case LD:
			c->mem_in_use++;
			setEndExCycle(i,c->cycle+c->mem_delay -1);
			c->write_cdb_mem = max(c->cycle + c->mem_delay, c->write_cdb_mem + 1);
			setWriteCDBCycle(i, c->write_cdb_mem);
			setResult(i,getMemoryI(getImm(i)));
			break;
		case ST:
			c->mem_in_use++;
			setEndExCycle(i,c->cycle+c->mem_delay -1);
			c->write_cdb_mem = max(c->cycle + c->mem_delay, c->write_cdb_mem + 1);
			setWriteCDBCycle(i, c->write_cdb_mem);
			setMemoryI(getImm(i),Vk);
			break;
		case ADD:
			c->add_in_use++;
			setEndExCycle(i,c->cycle+c->add_delay -1);
			c->write_cdb_add = max(c->cycle + c->mem_delay, c->write_cdb_add + 1);
			setWriteCDBCycle(i,c->write_cdb_add);
			setResult(i,Vj+Vk);
			break;
		case SUB:
			c->add_in_use++;
			setEndExCycle(i,c->cycle+c->add_delay -1);
			c->write_cdb_add = max(c->cycle + c->mem_delay, c->write_cdb_add + 1);
			setWriteCDBCycle(i,c->write_cdb_add);
			setResult(i,Vj-Vk);
			printf("in execute- result is %f - DEBUG 2\n",i->result);
					fflush(NULL);
			break;
		case MULT:
			c->mul_in_use++;
			setEndExCycle(i,c->cycle+c->mul_delay -1);
			c->write_cdb_mul = max(c->cycle + c->mem_delay, c->write_cdb_mul + 1);
			setWriteCDBCycle(i, c->write_cdb_mul);
			setResult(i,Vj*Vk);
			break;
		case DIV:
			c->div_in_use++;
			setEndExCycle(i,c->cycle+c->div_delay -1);
			c->write_cdb_div = max(c->cycle + c->mem_delay, c->write_cdb_div + 1);
			setWriteCDBCycle(i, c->write_cdb_div);
			setResult(i,Vj/Vk);
			break;
	}

}

void executeReadyAddSubInst(CPU c){
	int i = 0;
	for(i = 0; i < c->stations->numOfAddStations; i++){
		if(isBusy(c->stations->addStations[i]) && getIsReady(c->stations->addStations[i]) && c->add_nr_units > c->add_in_use){
			Instruction inst = getIssuedInstructionByIndex(c->queue, getIndexFromRsStation(c->stations->addStations[i]));
			if(getIssueCycle(inst) < c->cycle){
				setInExecution(c->stations->addStations[i]);
				execute(c, inst, getValuej(c->stations->addStations[i]), getValuek(c->stations->addStations[i]));
			}
		}
	}
}

void executeReadyMulInst(CPU c){
	int i = 0;
	for(i = 0; i < c->stations->numOfMulStations; i++){
		printf("in executeReadyMulInst- station %d - DEBUG 2\n",i);
		fflush(NULL);
		if(isBusy(c->stations->mulStations[i]) && getIsReady(c->stations->mulStations[i]) && c->mul_nr_units > c->mul_in_use){
			Instruction inst = getIssuedInstructionByIndex(c->queue, getIndexFromRsStation(c->stations->mulStations[i]));
			if(getIssueCycle(inst) < c->cycle){
				setInExecution(c->stations->mulStations[i]);
				execute(c, inst, getValuej(c->stations->mulStations[i]), getValuek(c->stations->mulStations[i]));
			}
		}
	}
}

void executeReadyDivInst(CPU c){
	int i = 0;
	for(i = 0; i < c->stations->numOfDivStations; i++){
		if(isBusy(c->stations->divStations[i]) && getIsReady(c->stations->divStations[i]) && c->div_nr_units > c->div_in_use){
			Instruction inst = getIssuedInstructionByIndex(c->queue, getIndexFromRsStation(c->stations->divStations[i]));
			if(getIssueCycle(inst) < c->cycle){
				setInExecution(c->stations->divStations[i]);
				execute(c, inst, getValuej(c->stations->divStations[i]), getValuek(c->stations->divStations[i]));
			}
		}
	}
}

int canLoadOrStore(CPU c, int index, int add, int opcode){
	Instruction inst;
	int i;
	for(i = 0; i < c->stations->numOfStoreBuffer; i++){
		if(!isBusy(c->stations->storeBuffer[i])){
			continue;
		}
		inst = getIssuedInstructionByIndex(c->queue, getIndexFromRsStation(c->stations->storeBuffer[i]));
		if(add == getImm(inst) && index > getIndex(inst)){
			return 0;
		}
	}
	if(opcode == ST){
		for(i = 0; i < c->stations->numOfLoadBuffer; i++){
			if(!isBusy(c->stations->loadBuffer[i])){
				continue;
			}
			inst = getIssuedInstructionByIndex(c->queue, getIndexFromRsStation(c->stations->loadBuffer[i]));
			if(add == getImm(inst) && index > getIndex(inst)){
				return 0;
			}
		}
	}
	return 1;
}

int executeReadyLoadStoreInst(CPU c){
	int i = 0, issueCycle, add, indexL, indexS;
	Instruction loadInst = NULL;
	Instruction storeInst = NULL;
	float storeVal = -1;
	for(i = 0; i < c->stations->numOfLoadBuffer; i++){
		if(isBusy(c->stations->loadBuffer[i]) && getIsReady(c->stations->loadBuffer[i])){
			loadInst = getIssuedInstructionByIndex(c->queue, getIndexFromRsStation(c->stations->loadBuffer[i]));
			issueCycle = getIssueCycle(loadInst);
			add = getImm(loadInst);
			indexL = getIndex(loadInst);
			if(issueCycle < c->cycle && canLoadOrStore(c,indexL, add, LD)){
				break;
			}
			loadInst = NULL;
		}
	}
	for(i = 0; i < c->stations->numOfStoreBuffer; i++){
		if(isBusy(c->stations->storeBuffer[i]) && getIsReady(c->stations->storeBuffer[i])){
			storeInst = getIssuedInstructionByIndex(c->queue, getIndexFromRsStation(c->stations->storeBuffer[i]));
			issueCycle = getIssueCycle(storeInst);
			add = getImm(storeInst);
			indexS = getIndex(storeInst);
			if(issueCycle < c->cycle && canLoadOrStore(c,indexS, add, LD)){
				storeVal = getValuek(c->stations->storeBuffer[i]);
				break;
			}
			storeInst = NULL;
		}
	}
	if(loadInst == NULL && storeInst == NULL){
		return 0;
	}
	if(!storeInst){
		execute(c, loadInst, -1, -1);
		return 1;
	}
	if(!loadInst){
		execute(c, storeInst, -1, storeVal);
		return 1;
	}
	if(indexL < indexS){
		execute(c, loadInst, -1, -1);
		return 1;
	}
	execute(c, storeInst, -1, storeVal);
	return 1;
}

void startExecution(CPU c){
	if(c->add_in_use < c->add_nr_units){
		printf("in startExecution - START ADD DEBUG \n");
		fflush(NULL);
		executeReadyAddSubInst(c);
	}
	if(c->mul_in_use < c->mul_nr_units)
		executeReadyMulInst(c);
	if(c->div_in_use < c->div_nr_units)
		executeReadyDivInst(c);
	if(c->mem_in_use == 0)
		executeReadyLoadStoreInst(c);
}

int issueInstruction(CPU c){
	int index;
	Instruction inst = getNonIssuedInstruction(c->queue);
	//printf("in issueInstruction - after getNonIssuedInstruction -  %08X - DEBUG\n", inst->instruction);
	//	fflush(NULL);
	if(getOpcode(inst) == HALT){
		c->halt = 1;
		destroyNode(removeFromNonIssuedQueue(c->queue));
		return 1;
	}
	index = isStationFree(c->stations, getOpcode(inst));
	//printf("in issueInstruction - after isStationFree -  %d - DEBUG\n", index);
	//		fflush(NULL);
	if(index == -1){
		return 0;
	}
	setIssueCycle(inst, c->cycle);
	addIssueInstruction(c->queue, removeFromNonIssuedQueue(c->queue));
	//printf("in issueInstruction - after addIssueInstruction -  %d - DEBUG\n", index);
	//			fflush(NULL);
	insertInstruction(c->stations, index, inst, c->regs);
	return 1;
}

void createRegout(CPU c){
	FILE *fRegout;
	int i = 0;
	if(c->regout == NULL){ //check for valid filename
		return;
	}
	fRegout = fopen(c->regout, "w");
	if(fRegout == NULL){ //check if file exist
			return;
	}
	for(i = 0; i< REGISTER_NUM; i++){
		float f;
		getValue(c->regs[i], &f);
		fprintf(fRegout, "%.5f\n", f);
		destroyRegister(c->regs[i]);
	}
	fclose(fRegout);
	//free(c->regs);
}

void createTraceinst(CPU c){
	FILE *fTraceinst;
	if(c->traceInst == NULL){ //check for valid filename
		return;
	}
	fTraceinst = fopen(c->traceInst, "w");
	if(fTraceinst == NULL){ //check if file exist
			return;
	}
	InstructionNode instNode = removeFromIssuedQueue(c->queue);
	while(instNode != NULL){
		Instruction inst = instNode->inst;
		fprintf(fTraceinst, "%08X %d %s %d %d %d %d\n", inst->instruction, inst->index, getStationName(inst), inst->issueCycle, inst->startExCycle, inst->endExCycle, inst->writeCDBCycle);
		destroyNode(instNode);
		instNode = removeFromIssuedQueue(c->queue);
	}
	fclose(fTraceinst);
}

void destroyCPU(CPU c){
	if(c != NULL){
		destroyReservationStationTable(c->stations);
		destroyInstructionQueue(c->queue);
		free(c);
	}
}

void runCPU(CPU c){
	int pc = 0;
	importMemory(c->memin);
	if(addInstruction(c->queue, pc, getMemoryInstruction(pc))){
		pc++;
	}
	else{
		return;
	}
	if(addInstruction(c->queue, pc, getMemoryInstruction(pc))){
		pc++;
	}
	/*printf("in runCPU- pc %d - DEBUG 2\n",pc);
	Instruction inst = c->queue->nonIssueInsts->inst;
	printf("in runCPU- %08X %d %d %d %d %d - DEBUG\n", inst->instruction, inst->index, inst->issueCycle, inst->startExCycle, inst->endExCycle, inst->writeCDBCycle);
	fflush(NULL);
	printf("in runCPU- after addInstruction - DEBUG 2\n");
	fflush(NULL);*/
	while(/*c->done == 0 */c->cycle < 10){
		c->cycle++;
		//printf("in runCPU- in while %d - DEBUG 2\n",c->cycle);
		//fflush(NULL);
		issueInstruction(c);
		issueInstruction(c);
		if(addInstruction(c->queue, pc, getMemoryInstruction(pc))){
			pc++;
		}
		if(addInstruction(c->queue, pc, getMemoryInstruction(pc))){
			pc++;
		}
		//printf("in runCPU- station %d %d %d %d %s- DEBUG 2\n",c->stations->addStations[0]->index, c->stations->addStations[0]->busy, c->stations->addStations[0]->ready, c->stations->addStations[0]->opcode, c->stations->addStations[0]->name);
		//fflush(NULL);
		//printf("in runCPU- station %d %d %d %d %s- DEBUG 2\n",c->stations->addStations[1]->index, c->stations->addStations[1]->busy, c->stations->addStations[1]->ready, c->stations->addStations[1]->opcode, c->stations->addStations[1]->name);
		//fflush(NULL);
		//printf("in runCPU- station %d %d %d %d %s- DEBUG 2\n",c->stations->mulStations[0]->index, c->stations->mulStations[0]->busy, c->stations->mulStations[0]->ready, c->stations->mulStations[0]->opcode, c->stations->mulStations[0]->name);
		//fflush(NULL);
		startExecution(c);


		writeCDB(c);
		printf("in runCPU- after writeCDB %d - DEBUG 2\n",c->cycle);
		fflush(NULL);

	}
	printf("in runCPU- after while- DEBUG 2\n");
	fflush(NULL);

	exportMemory(c->memout);
	createRegout(c);
	createTraceinst(c);

	printf("Simulation Done!!!!\n");
}
