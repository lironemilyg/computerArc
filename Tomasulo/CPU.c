/*
 * CPU.c
 *
 *  Created on: 16 Dec 2017
 *      Author: lirongazit
 */

#include "InstructionQueue.c"

#define LD 0
#define ST 1
#define ADD 2
#define SUB 3
#define MULT 4
#define DIV 5
#define HALT 6

typedef struct cpu{
	int cycle;
	int lastWrite;
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
	int load_in_use;
	int store_in_use;
	char* cfg, memin, memout, regout, traceInst, traceCDB;
	//RegisterEntry* regs;
	//Memory memory;
	//ReservationStationTable stations;
	//InstructionQueue queue;
}*CPU;

void execute(CPU c, Instruction i,float Vj, float Vk){
	setStartExCycle(i,c->cycle);
	switch(i->opcode){
		case LD:
			c->load_in_use++;
			setEndExCycle(i,c->cycle+c->mem_delay -1);
			setWriteCDBCycle(i,c->cycle+c->mem_delay);
			setResult(i,getMemoryI(getImm(i)));
			break;
		case ST:
			c->store_in_use++;
			setEndExCycle(i,c->cycle+c->mem_delay -1);
			setWriteCDBCycle(i,c->cycle+c->mem_delay);
			setMemoryI(getImm(i),Vk);
			break;
		case ADD:
			c->add_in_use++;
			setEndExCycle(i,c->cycle+c->add_delay -1);
			setWriteCDBCycle(i,c->cycle+c->add_delay);
			setResult(i,Vj+Vk);
			break;
		case SUB:
			c->add_in_use++;
			setEndExCycle(i,c->cycle+c->add_delay -1);
			setWriteCDBCycle(i,c->cycle+c->add_delay);
			setResult(i,Vj-Vk);
			break;
		case MULT:
			c->mul_in_use++;
			setEndExCycle(i,c->cycle+c->mul_delay -1);
			setWriteCDBCycle(i,c->cycle+c->mul_delay);
			setResult(i,Vj*Vk);
			break;
		case DIV:
			c->div_in_use++;
			setEndExCycle(i,c->cycle+c->div_delay -1);
			setWriteCDBCycle(i,c->cycle+c->div_delay);
			setResult(i,Vj/Vk);
			break;
	}

}
