//Vector.c

#include <stdio.h>
#include <stdlib.h>
#include "Vector.h"

void vector_init(Vector * vector) {
	vector->size=0;
	vector->capacity=VECTOR_INIT_CAPACITY;

	vector->data=malloc(sizeof(int) * vector->capacity);
}

void vector_append(Vector * vector, int value) {
	vector_double_capacity(vector);
	vector->data[vector->size++] = value;
}

int vector_get(Vector * vector, int index) {
	if (index >= vector->size || index < 0) {
		printf("Index %d out of bounds for vector of size %d\n", index, vector->capacity);
	}

	return vector->data[index];
}

void vector_set(Vector * vector, int index, int value) {
	while(index >= vector->size) {
		vector_append(vector, 0);
	}

	vector->data[index] = value;
}

void vector_double_capacity(Vector * vector) {
	if (vector->size >= vector->capacity) {
		vector->capacity *= 2;
		vector->data = realloc(vector->data, sizeof(int) * vector->capacity);
	}
}

void vector_free(Vector * vector) {
	free(vector->data);
}