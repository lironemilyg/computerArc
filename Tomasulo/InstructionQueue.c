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
	struct instructionNode* prev;
}*InstructionNode;

typedef struct instructionQueue{
	InstructionNode nonIssueInsts;
	InstructionNode issueInsts;
	InstructionNode nonIssueInstsTail;
	InstructionNode issueInstsTail;
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

int addInstruction(InstructionQueue q, int index, int instFromMem){
	if(q->nonIssueSize >= MAX_INST_QUEUE)
		return 0;
	q->nonIssueSize++;
	Instruction i = initInstruction(index, instFromMem);
	InstructionNode in = (InstructionNode)malloc(sizeof(InstructionNode*)+sizeof(*i));
	if(in == NULL)
		return 0;
	in->prev = NULL;
	in->inst = i;
	if(q->nonIssueSize == 1){
		q->nonIssueInsts = in;
		q->nonIssueInstsTail = in;
		in->next = NULL;
	}
	else{
		in->next = q->nonIssueInsts;
		q->nonIssueInsts->prev = in;
		q->nonIssueInsts = in;
	}
	/*printf("in addInstruction- %08X - DEBUG\n", i->instruction);
			fflush(NULL);
			printf("in addInstruction- %08X - DEBUG\n", in->inst->instruction);
					fflush(NULL);*/
	return 1;
}

int addIssueInstruction(InstructionQueue q, InstructionNode in){
	//printf("in addIssueInstruction  -  %08X - DEBUG\n", in->inst->instruction);
	//fflush(NULL);
	q->issueSize++;
	in->prev = NULL;
	if(q->issueSize == 1){
		q->issueInsts = in;
		q->issueInstsTail = in;
		in->next = NULL;
	}
	else{
		in->next = q->issueInsts;
		q->issueInsts->prev = in;
		q->issueInsts = in;
	}
	//printf("in addIssueInstruction - end -  %08X - DEBUG\n", q->issueInsts->inst->instruction);
	//	fflush(NULL);
	return 1;
}

Instruction getNonIssuedInstruction(InstructionQueue q){
	return q->nonIssueInstsTail->inst;
}

Instruction getIssuedInstructionByIndex(InstructionQueue q, int index){
	InstructionNode curr = q->issueInsts;
	while(curr != NULL){
		if(getIndex(curr->inst) == index){
			return curr->inst;
		}
		curr = curr->next;
	}
	return NULL;
}

InstructionNode removeFromIssuedQueue(InstructionQueue q){
	InstructionNode curr = q->issueInstsTail;
	if(curr == NULL){
		return NULL;
	}
	q->nonIssueInstsTail = curr->prev;
	if(curr->prev != NULL){
		curr->prev->next = NULL;
		curr->prev = NULL;
	}
	q->issueSize--;
	return curr;
}

InstructionNode removeFromNonIssuedQueue(InstructionQueue q){
	InstructionNode curr = q->nonIssueInstsTail;
	q->nonIssueInstsTail = curr->prev;
	if(curr->prev != NULL){
		curr->prev->next = NULL;
		curr->prev = NULL;
	}
	q->nonIssueSize--;
	//printf("in removeFromNonIssuedQueue - -  %08X - DEBUG\n", curr->inst->instruction);
	//		fflush(NULL);
	return curr;
}

void destroyNode(InstructionNode inst){
	destroyInstruction(inst->inst);
	free(inst);
}

void destroyInstructionQueue(InstructionQueue q){
	if(q != NULL){
		free(q->nonIssueInsts);
		free(q->issueInsts);
		free(q);
	}
}
