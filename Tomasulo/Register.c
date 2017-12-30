/*
 * Register.c
 *
 *  Created on: 9 Dec 2017
 *      Author: lirongazit
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef struct reg{
	float val;
	int rValid;
	char* tag;
}*Register;

Register initRegister(int index){
	Register r = (Register)malloc(sizeof(Register*) + 8);
	if(r == NULL)
		return r;
	r->val = (float)index;
	r->tag = NULL;
	r->rValid = 1;
	return r;
}

void destroyRegister(Register r){
	if(r != NULL){
		if(r->tag != NULL){
			free(r->tag);
		}
	}
}

int isValid(Register r){
	if(r != NULL){
		return r->rValid;
	}
	return 0;
}

int getValue(Register r, float* val){
	if(r != NULL){
		*val = r->val;
		return 1;
	}
	return 0;
}

int getTag(Register r, char** tag){
	if(r != NULL){
		*tag = r->tag;
		return 1;
	}
	return 0;
}

int setValue(Register r, float val){
	if(r != NULL){
		r->val = val;
		return 1;
	}
	return 0;
}

int setTag(Register r, char* tag){
	if(r != NULL){
		if(r->tag != NULL){
			//free(r->tag);
			r->rValid = 1;
		}
		if(tag != NULL){
			size_t size = strlen(tag) + 1;
			r->tag = (char*) malloc(size);
			memset(r->tag,'\0',size);
			strncpy(r->tag,tag,size);
			r->rValid = 0;
		}
		else
			r->tag = NULL;
		return 1;
	}
	return 0;
}

