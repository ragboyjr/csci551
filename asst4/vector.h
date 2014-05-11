#ifndef _VECTOR_H
#define _VECTOR_H

#include <assert.h>

typedef struct _vector_t {
    double * data;
    int size;
} vector_t;

vector_t * vector_create_with_data(int size, double * data);
vector_t * vector_create(int size);
vector_t * vector_clone(vector_t * other);
vector_t * vector_create_rand(int size);

void vector_alloc_data(vector_t * this);
void vector_destroy(vector_t * this);
void vector_print(vector_t * this);

double vector_dot_product(vector_t * this, vector_t * other);
/* this = a - b */
void vector_subtract(vector_t * this, vector_t * a, vector_t * b);
double vector_l2norm(vector_t * this);

#define vector_foreach(v, i) assert(v);\
for (i = 0; i < (v)->size; i++)

#endif
