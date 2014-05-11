#ifndef _VECTOR_H
#define _VECTOR_H

#include <assert.h>

typedef struct {
    double * data;
    int size;
} vector_t;

vector_t * vector_create_with_data(int size, double * data);
vector_t * vector_create(int size);
vector_t * vector_create_rand(int size);

void vector_alloc_data(vector_t * this);
void vector_destroy(vector_t * this);
void vector_print(vector_t * this);

#define vector_foreach(v, i) assert(v);\
for (i = 0; i < (v)->size; i++)

#endif
