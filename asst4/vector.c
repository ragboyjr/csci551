#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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

vector_t * vector_clone(vector_t * other)
{
    vector_t * this  = malloc(sizeof(vector_t));
    
    this->data = malloc(sizeof(double) * other->size),
    this->size = other->size;
    
    /* copy over the data */
    memcpy(this->data, other->data, sizeof(double) * other->size);
    
    return this;
}

vector_t * vector_create_rand(int size)
{
    int i;
    vector_t * this = vector_create(size);
    
    vector_foreach(this, i) {
        this->data[i] = drand48() * 2e10 - 1e10;
        //this->data[i] = rand() % 5 + 1;
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

double vector_dot_product(vector_t * this, vector_t * other)
{
    int i;
    double res = 0.0;
    
    assert(this->size == other->size);
    
    vector_foreach(this, i) {
        res += this->data[i] * other->data[i];
    }
    
    return res;
}

void vector_subtract(vector_t * this, vector_t * a, vector_t * b)
{
    int i;
    vector_foreach(this, i) {
        this->data[i] = a->data[i] - b->data[i];
    }
}

double vector_l2norm(vector_t * this)
{
    int i;
    double res = 0.0;
    
    vector_foreach(this, i) {
        res += pow(this->data[i], 2);
    }
    
    return fabs(sqrt(res));
}
