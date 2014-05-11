#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
vector_t * vector_create_with_data(int size, double * data)
{
    vector_t * this  = malloc(sizeof(vector_t));
    
    this->data = data,
    this->size = size;
    return this;
}

vector_t * vector_create(int size)
{
    return vector_create_with_data(size, calloc(size, sizeof(double)));
}

vector_t * vector_create_rand(int size)
{
    int i;
    vector_t * this = vector_create(size);
    
    vector_foreach(this, i) {
        this->data[i] = drand48() * 2e10 - 1e10;
    }
    
    return this;
}

void vector_alloc_data(vector_t * this)
{
    this->data = calloc(this->size, sizeof(double));
}

void vector_destroy(vector_t * this)
{
    if (this->data) {
        free(this->data);
    }
    
    free(this);
}

void vector_print(vector_t * this)
{
    int i;
    printf("[");
    
    /* loop over the matrix */
    vector_foreach(this, i) {
        printf("%10.2lf, ", this->data[i]); 
    }
    
    puts("]");
}
