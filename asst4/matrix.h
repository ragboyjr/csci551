#ifndef _MATRIX_H
#define _MATRIX_H

#include <assert.h>

/* struct for storing matrices */
typedef struct _matrix_t {
    double ** data;
    int rows,
        cols;
} matrix_t;

matrix_t * matrix_create(int rows, int cols);
matrix_t * matrix_clone(matrix_t * other);
matrix_t * matrix_create_rand(int rows, int cols);

void matrix_alloc_data(matrix_t * this);
void matrix_destroy(matrix_t * this);
void matrix_print(matrix_t * this);

#define matrix_idx(m, r, c) (m)->data[(r) * (m)->cols + (c)]
#define matrix_foreach(m, i, j) assert(m);\
for (i = 0, j = 0; i < m->rows && j < m->cols; (j == m->cols - 1) ? j = 0, i++ : j++)

#endif
