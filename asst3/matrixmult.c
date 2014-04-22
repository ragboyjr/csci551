#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>
#include <mpi.h>


/* global vars */
static int my_rank;
static int comm_sz;

/* matrix defs */

#define matrix_idx(m, r, c) (m)->data[(r) * (m)->cols + (c)]
#define matrix_foreach(m, i, j) assert(m);\
for (i = 0, j = 0; i < m->rows && j < m->cols; (j == m->cols - 1) ? j = 0, i++ : j++)

typedef struct _matrix_t {
    double * data;
    int rows,
        cols;
} matrix_t;

typedef enum {
    MATRIX_MULT_IJK,
    MATRIX_MULT_IKJ,
    MATRIX_MULT_KIJ,
} matrix_mult_t;

typedef struct {
    matrix_t * A,
             * B,
             * C;
} matrix_data_t;

matrix_t * matrix_create_with_data(int rows, int cols, double * data)
{
    matrix_t * this = malloc(sizeof(matrix_t));
    
    this->rows = rows,
    this->cols = cols,
    this->data = data;
    
    return this;
}

matrix_t * matrix_create(int rows, int cols)
{
    return matrix_create_with_data(rows, cols, calloc(sizeof(double), rows * cols));
}

void matrix_alloc_data(matrix_t * this)
{
    this->data = calloc(sizeof(double), this->rows * this->cols);
}

void matrix_destroy(matrix_t * this)
{
    if (this->data) {
        free(this->data);
    }
    free(this);
}

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
        
        printf("%10.2lf, ", matrix_idx(this, i, j));
        
        if (j == this->cols - 1) {
            printf("\n");
        }      
    }
    
    puts(")");
}

void matrix_mult_by_type(matrix_t ** C_ref, matrix_t * A, matrix_t * B, matrix_mult_t type)
{
    int i, j, k, n = A->cols;
    
    matrix_t * C = *C_ref;
    
    /* these need to be true */
    assert(A);assert(B);assert(C);
    assert(A->cols == B->rows);
    assert(C->rows == A->rows);
    assert(C->cols == B->cols);
    
    /* with the help of http://www.daniweb.com/software-development/csharp/code/355645/optimizing-matrix-multiplication */
    
    switch (type)
    {
        case MATRIX_MULT_IJK:
            for (i = 0; i < A->rows; i++)
            {
                for (j = 0; j < B->cols; j++)
                {
                    for (k = 0; k < n; k++) {
                        matrix_idx(C, i, j) += matrix_idx(A, i, k) * matrix_idx(B, k, j);
                    }
                }
            }
            break;
        case MATRIX_MULT_IKJ:
            for (i = 0; i < A->rows; i++)
            {
                for (k = 0; k < n; k++)
                {
                    for (j = 0; j < B->cols; j++) {
                        matrix_idx(C, i, j) += matrix_idx(A, i, k) * matrix_idx(B, k, j);
                    }
                }
            }
            break;
        case MATRIX_MULT_KIJ:
            for (k = 0; k < n; k++)
            {
                for (i = 0; i < A->rows; i++)
                {
                    for (j = 0; j < B->cols; j++) {
                        matrix_idx(C, i, j) += matrix_idx(A, i, k) * matrix_idx(B, k, j);
                    }
                }
            }
            break;
    }
    
}

static void build_random_matrix(matrix_t ** M_ref)
{
    matrix_t * M = *M_ref;
    int i, j;
    
    matrix_foreach(M, i, j) {
        matrix_idx(M, i, j) = rand() % 20 - 10;
    }
}

static void fill_global_matrices_from_user_input(matrix_mult_t * mult_type, matrix_t ** A, matrix_t ** B)
{
    /* input must be in the form of
       <form>
       <n>
       <input_type>
       <A>
       <B>
       
       <form> = ijk, ikj, kij
       <n> = integer*/
    enum {
        SCAN_FORM,
        SCAN_INPUT_TYPE,
        SCAN_N,
        SCAN_A,
        SCAN_B,
        SCAN_END,
    } scan_state = SCAN_FORM;
    int j = 0, n = 0;
    char form_buf[4]   = {0},
         input_type[2] = {0};
    
    while (!feof(stdin) && scan_state != SCAN_END)
    {
        switch (scan_state)
        {
            case SCAN_FORM:
                fscanf(stdin, "%3s", form_buf);
                
                if (strcasecmp(form_buf, "ijk") == 0) {
                    *mult_type = MATRIX_MULT_IJK;
                }
                else if (strcasecmp(form_buf, "ikj") == 0) {
                    *mult_type = MATRIX_MULT_IKJ;
                }
                else if (strcasecmp(form_buf, "kij") == 0) {
                    *mult_type = MATRIX_MULT_KIJ;
                }
                else
                {
                    puts("error: expected matrix multiplication form of ijk, ikj, or kij");
                    exit(EXIT_FAILURE);
                }
                
                scan_state = SCAN_INPUT_TYPE;
                break; 
            case SCAN_INPUT_TYPE:
                fscanf(stdin, "%1s", input_type);
                
                *input_type = tolower(*input_type);
                
                if (*input_type != 'r' && *input_type != 'i')
                {
                    puts("error: input type needs to be of the values <i,r> for input or random");
                    exit(EXIT_FAILURE);
                }
                
                scan_state = SCAN_N;
                break;
            case SCAN_N:            
                fscanf(stdin, "%d", &n);
                
                if (n <= 1)
                {
                    puts("error: matrix n needs to be greater than 1");
                    exit(EXIT_FAILURE);
                }
                
                *A = matrix_create(n, n);
                *B = matrix_create(n, n);
                
                if (*input_type == 'r')
                {
                    build_random_matrix(A);
                    build_random_matrix(B);
                    scan_state = SCAN_END;
                }
                else
                {
                    j = n = n * n;
                    scan_state = SCAN_A;
                }
                
                break;
            case SCAN_A:
                if (j == 0)
                {
                    j = n;
                    scan_state = SCAN_B;
                }
                else
                {
                    /* fill the A matrix with proper data, n is the total count and j goes from
                       n to 0, n - j will go to 0 to n - 1 */
                    fscanf(stdin, "%lf", (*A)->data + (n - j));
                    j--;
                }
                break;
            case SCAN_B:
                if (j == 0)
                {
                    scan_state = SCAN_END;
                }
                else
                {
                    /* fill the B matrix with proper data, n is the total count and j goes from
                       n to 0, n - j will go to 0 to n - 1 */
                    fscanf(stdin, "%lf", (*B)->data + (n - j));
                    j--;
                }
                break;
            default:
                assert(0);
        }
    }
    
    if (scan_state != SCAN_END)
    {
        puts("EOF was hit too early");
        exit(EXIT_FAILURE);    
    }
}

// static void fill_matrix(matrix_mult_t * mult_type, matrix_t ** A, matrix_t ** B)
// {
//     *mult_type = MATRIX_MULT_IJK;
//     
//     *A = matrix_create(2, 3);
//     *B = matrix_create(3, 2);
//     
//     matrix_idx(*A, 0, 0) = 1;
//     matrix_idx(*A, 0, 1) = 2;
//     matrix_idx(*A, 0, 2) = 3;
//     matrix_idx(*A, 1, 0) = 4;
//     matrix_idx(*A, 1, 1) = 5;
//     matrix_idx(*A, 1, 2) = 6;
//     
//     matrix_idx(*B, 0, 0) = 1;
//     matrix_idx(*B, 0, 1) = 2;
//     matrix_idx(*B, 1, 0) = 3;
//     matrix_idx(*B, 1, 1) = 4;
//     matrix_idx(*B, 2, 0) = 5;
//     matrix_idx(*B, 2, 1) = 6;
// }

static void distribute_global_data(matrix_mult_t * mult_type, matrix_data_t * global, matrix_data_t * local)
{    
    int n, a_size;
    /* Broadcast mult_type, the global B matrix. then scatter the A matrix */
    MPI_Bcast(mult_type, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&global->B->rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    /* if B wasn't initialized, then we aren't rank 0 process */
    if (!global->B->data)
    {
        assert(my_rank != 0);
        global->B->cols = global->B->rows;
        matrix_alloc_data(global->B);
    }
    
    /* send the B matrix data */
    MPI_Bcast(global->B->data, global->B->rows * global->B->cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    /* build the size of the local A matrices */
    n = global->B->cols;
    
    if (n % comm_sz != 0)
    {
        printf("n is not perfectly divisible by the comm_sz of %d\n", comm_sz);
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    
    local->A->rows = n / comm_sz;
    local->A->cols = n;
    matrix_alloc_data(local->A);
    
    /* scatter the A matrix */
    a_size = local->A->rows * local->A->cols;
    MPI_Scatter(global->A->data, a_size, MPI_DOUBLE, local->A->data, a_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

static void main_initialize(int argc, char * argv[])
{
    srand(time(NULL));
    MPI_Init(&argc, &argv);
    
    /* Get my process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out how many processes are being used */
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
}

int main(int argc, char * argv[])
{
    matrix_mult_t mult_type;
    
    /* global and local matrix pointers */
    matrix_data_t global, local;
    int c_size, should_print = 1;
    
    double t_start, t_finish;
    
    memset(&global, 0, sizeof(matrix_data_t));
    memset(&local, 0, sizeof(matrix_data_t));

    /* run basic initialization for the program */
    main_initialize(argc, argv);
    
    if (argc > 1 && strcmp("0", argv[1]) == 0) {
        should_print = 0;
    }
    
    if (my_rank == 0) {
        fill_global_matrices_from_user_input(&mult_type, &global.A, &global.B);
        global.C = matrix_create(global.A->rows, global.B->cols);
    }
    else {
        /* create blank matrices */
        global.A = matrix_create_with_data(0, 0, NULL);
        global.B = matrix_create_with_data(0, 0, NULL);
        global.C = matrix_create_with_data(0, 0, NULL);
    }
    
    /* create blank matrices */
    local.A = matrix_create_with_data(0, 0, NULL);

    t_start = MPI_Wtime();

    distribute_global_data(&mult_type, &global, &local);

    /* create the local C matrix */
    local.C = matrix_create(local.A->rows, global.B->cols);
        
    /* perform local multiplication */
    matrix_mult_by_type(&local.C, local.A, global.B, mult_type);
        
    c_size = local.C->rows * local.C->cols;
    MPI_Gather(local.C->data, c_size, MPI_DOUBLE, global.C->data, c_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    t_finish = MPI_Wtime();
    
    if (my_rank == 0)
    {
        /* print the stats */
        printf("running on %d processor(s)\n", comm_sz);
        printf("elapsed time = %.7lf seconds\n", t_finish - t_start);
        
        if (should_print) {
            matrix_print(global.C);
        }
    }
    
    MPI_Finalize();

    /* destroy the matrices */
    matrix_destroy(global.A);
    matrix_destroy(global.B);
    matrix_destroy(global.C);
    matrix_destroy(local.A);
    matrix_destroy(local.C);
    return 0;
}
