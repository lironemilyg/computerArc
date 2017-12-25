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
	char* tag;
}*Register;

Register initRegister(int index){
	Register r = (Register)malloc(sizeof(Register*) + 8);
	if(r == NULL)
		return r;
	r->val = (float)index;
	r->tag = NULL;
	return r;
}

void destroyRegister(Register r){
	if(r != NULL){
		if(r->tag != NULL){
			free(r->tag);
		}
		//free(r);
		//r = NULL;
	}
}

int isValid(Register r){
	if(r != NULL){
		if(r->tag == NULL){
			return 1;
		}
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
		if(r->tag){
			free(r->tag);
			r->tag = NULL;
		}
		if(tag != NULL){
			size_t size = strlen(tag);
			r->tag = (char*) malloc(size+1);
			strncpy(r->tag,tag,size);
		}
		return 1;
	}
	return 0;
}

