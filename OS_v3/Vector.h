//Vector.h 
//Author : Febi Agil Ifdillah
//Dynamic array like vector
//source : https://www.happybearsoftware.com/implementing-a-dynamic-array

#ifndef VECTOR_H
#define VECTOR_H

#define VECTOR_INIT_CAPACITY 50
#include "Boolean.h"

typedef struct {
	int size;				//slot used so far
	int capacity;			//total available slots
	int *data;			// array of integer we're storing
} Vector;

void vector_init(Vector * vector);

void vector_append(Vector * vector, int value);

int vector_get(Vector * vector, int index);

void vector_set(Vector * vector, int index, int value);

void vector_double_capacity(Vector * vector);

void vector_free(Vector * vector);

#endif