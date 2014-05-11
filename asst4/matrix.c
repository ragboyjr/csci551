#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>

matrix_t * matrix_create(int rows, int cols)
{
    int i;
    matrix_t * this = malloc(sizeof(matrix_t));
    
    this->rows = rows,
    this->cols = cols,
    this->data = malloc(sizeof(double *) * rows);
    
    for (i = 0; i < rows; i++) {
        this->data[i] = calloc(cols, sizeof(double));
    }
    
    return this;
}

matrix_t * matrix_create_rand(int rows, int cols)
{
    matrix_t * this = matrix_create(rows, cols);
    
    int i,j;
    matrix_foreach(this, i, j) {
        this->data[i][j] = drand48() * 2e10 - 1e10;
    }
    
    return this;
}

/* some matrices need to allocate their data after initialization */
void matrix_alloc_data(matrix_t * this)
{
    this->data = calloc(this->rows * this->cols, sizeof(double));
}

/* freeup the matrix data */
void matrix_destroy(matrix_t * this)
{
    if (this->data) {
        free(this->data);
    }
    free(this);
}

/* print out the matrix */
void matrix_print(matrix_t * this)
{
    int i, j;
    puts("(");
    
    /* loop over the matrix */
    matrix_foreach(this, i, j)
    {
        if (j == 0) {
            printf("    ");
        }
        
        printf("%10.2lf, ", this->data[i][j]);
        
        if (j == this->cols - 1) {
            printf("\n");
        }      
    }
    
    puts(")");
}
