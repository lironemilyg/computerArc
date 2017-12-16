/*
 * InstructionQueue.c
 *
 *  Created on: 16 Dec 2017
 *      Author: lirongazit
 */

#include <stdlib.h>
#include "Instruction.c"
#define MAX_INST_QUEUE 16

typedef struct instructionNode{
	Instruction inst;
	struct instructionNode* next;
}*InstructionNode;

typedef struct instructionQueue{
	InstructionNode nonIssueInsts;
	InstructionNode issueInsts;
	InstructionNode nonIssueInstsTail;
	int nonIssueSize;
	int issueSize;
}*InstructionQueue;

InstructionQueue initInstructionQueue(){
	InstructionQueue queue = (InstructionQueue)malloc(sizeof(InstructionNode*)*4096);
	if(queue == NULL)
		return queue;
	queue->issueSize = 0;
	queue->nonIssueSize = 0;
	return queue;
}

int addInstruction(InstructionQueue q, Instruction i){
	if(q->nonIssueSize >= MAX_INST_QUEUE)
		return 0;
	q->nonIssueSize++;
	InstructionNode in = (InstructionNode)malloc(sizeof(InstructionNode*));
	if(in == NULL)
		return 0;
	in->inst = i;
	if(q->nonIssueSize == 1){
		q->nonIssueInsts = in;
		q->nonIssueInstsTail = in;
		in->next = NULL;
	}
	else{
		in->next = q->nonIssueInsts;
		q->nonIssueInsts = in;
	}
	return 1;
}

int issueInstruction(){

}
