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
    srand(time(NULL));
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

static void perform_partial_pivot(matrix_t * A, vector_t * b, int k)
{
    int pivot_row = find_pivot_row(A, k, k);
    double * tmp,
           tmp_d;
    
    if (pivot_row == k) {
        return; /* no need to pivot */
    }
            
    /* swap the pivot row and the current */
    tmp                 = A->data[pivot_row];
    A->data[pivot_row]  = A->data[k];
    A->data[k]          = tmp;
    
    tmp_d               = b->data[pivot_row];
    b->data[pivot_row]  = b->data[k];
    b->data[k]          = tmp_d;
}

static void guassian_eliminate_idx(matrix_t * A, vector_t * b, int k, int cur_row_idx)
{
    double * top_row    = A->data[k],
           * cur_row    = A->data[cur_row_idx],
           scaled_val   = (cur_row[k] / top_row[k]);
    int j;
    
    /* loop over the columns of the matrix, we don't need to worry about previous columns
       because they have already been eliminated */    
    for (j = k; j < A->cols; j++) {
        cur_row[j] = top_row[j] * scaled_val - cur_row[j];
    }
    
    /* update b also */
    b->data[cur_row_idx] = b->data[k] * scaled_val - b->data[cur_row_idx];
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
    
    /* k loops over the diagonal */
    for (k = 0; k < A->rows; k++)
    {
        //t_start = get_time_in_sec() * 1000; /* milliseconds */       
        
        perform_partial_pivot(A, b, k);
                
        /* perform gaussian elimination on all of the rows of this col */
        for (i = k + 1; i < A->rows; i++) {
            guassian_eliminate_idx(A, b, k, i);
        }
        
        //t_end = get_time_in_sec() * 1000;
        //printf("k = %d - %.8lfms\n", k, t_end - t_start);
    }
}

static void perform_back_substitution(matrix_t * A, vector_t * x, vector_t * b)
{
    int i, k;
    
    for (k = A->rows - 1; k >= 0; k--)
    {
        x->data[k] = b->data[k] / A->data[k][k];
        
        for (i = k - 1; i >= 0; i--) {
            b->data[i] = b->data[i] - A->data[i][k] * x->data[k];
        }
    }
}

static void matrix_mult_vector(matrix_t * A, vector_t * x, vector_t * b)
{
    /* Ax = b */
    int i;
    vector_t * tmp_v = vector_create_with_data(x->size, NULL);
    
    for (i = 0; i < A->rows; i++)
    {
        /* treat A's row as a vector */
        tmp_v->data = A->data[i];
        
        /* run a dot product and store in b */
        b->data[i] = vector_dot_product(tmp_v, x);
    }
    
    tmp_v->data = NULL;
    vector_destroy(tmp_v);
}

int main(int argc, char * argv[])
{
    int n, p;
    matrix_t * A,
             * A_orig;
    vector_t * b,
             * b_orig,
             * x,
             * check_res_mat;
             
    double t_start, t_end;
    
    
    get_input_params(argc, argv, &n, &p);
    initialize_global_state();
    
    /* initialize data */
    A_orig          = matrix_create_rand(n, n),
    A               = matrix_clone(A_orig),
    b_orig          = vector_create_rand(n),
    b               = vector_clone(b_orig),
    x               = vector_create(n),
    check_res_mat   = vector_create(n);
    
    t_start = get_time_in_sec();
    
    perform_gaussian_elimination_on_matrix(A, b);
    perform_back_substitution(A, x, b);
    
    t_end  = get_time_in_sec();
    
    matrix_print(A);
    
    /* calculate Ax - b and find it's l2norm to test for correctness */
    matrix_mult_vector(A_orig, x, check_res_mat);
    
    vector_subtract(check_res_mat, check_res_mat, b_orig); /* c = c - b */
    
    printf("Performed Gaussian Elimination in %.12lfs\n", t_end - t_start);
    printf("Ax-b l2norm = %.6le\n", vector_l2norm(check_res_mat));
    
    puts("done");   
    return 0;
}
