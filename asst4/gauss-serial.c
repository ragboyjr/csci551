#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "vector.h"
#include "matrix.h"

static void get_input_params(int argc, char * argv[], int * n, int * p)
{
    if (argc < 3)
    {
        printf("usage: %s n p\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    *n = atoi(argv[1]);
    *p = atoi(argv[2]);
}

static void initialize_global_state()
{
    srand48(time(NULL));
}

/* is the topmost row, j is the column */
static int find_pivot_row(matrix_t * A, int i, int j)
{
    /* initialize to first row */
    double max_val = A->data[i][j];
    int max_idx = i;
    
    for (; i < A->rows; i++)
    {
        if (max_val >= A->data[i][j]) {
            continue;
        }
        
        /* update max val now */
        max_val = A->data[i][j];
        max_idx = i;
    }
    
    return max_idx;
}

static void perform_partial_pivot(matrix_t * A, int k)
{
    int pivot_row = find_pivot_row(A, k, k);
    double * tmp;
    
    if (pivot_row == k) {
        return; /* no need to pivot */
    }
            
    /* swap the pivot row and the current */
    tmp                 = A->data[pivot_row];
    A->data[pivot_row]  = A->data[k];
    A->data[k]          = tmp;
}

static void guassian_eliminate_idx(matrix_t * A, vector_t * b, int k, int cur_row_idx)
{
    double * top_row = A->data[k],
           * cur_row = A->data[cur_row_idx];
    int j;
    
    /* loop over the columns of the matrix, we don't need to worry about previous columns
       because they have already been eliminated */
    for (int j = k; j < A->cols; j++) {
        cur_row[j] = top_row[j] * (cur_row[k] / top_row[k]) - cur_row[j];
    }
}

static double get_time_in_sec()
{
    struct timeval tv; memset(&tv, 0, sizeof(tv));
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / (double) 1000000.0);
}

static void perform_gaussian_elimination_on_matrix(matrix_t * A, vector_t * b)
{
    int k, i;
    double t_start, t_end;
    
    //matrix_print(A);
    
    /* k loops over the diagonal */
    for (k = 0; k < A->rows; k++)
    {
        t_start = get_time_in_sec() * 1000; /* milliseconds */       
        
        perform_partial_pivot(A, k);
        
        /* perform gaussian elimination on all of the rows of this col */
        for (i = k + 1; i < A->rows; i++) {
            guassian_eliminate_idx(A, b, k, i);
        }
        
        t_end = get_time_in_sec() * 1000;
        printf("k = %d - %.8lfms\n", k, t_end - t_start);
    }
    
    //matrix_print(A);
}

int main(int argc, char * argv[])
{
    int n, p;
    matrix_t * A;
    vector_t * b,
             * x;
    
    
    get_input_params(argc, argv, &n, &p);
    initialize_global_state();
    
    /* initialize data */
    A = matrix_create_rand(n, n),
    b = vector_create_rand(n),
    x = vector_create_rand(n);
    
    puts("starting");
    
    perform_gaussian_elimination_on_matrix(A, b);
    
    puts("done");   
    return 0;
}
