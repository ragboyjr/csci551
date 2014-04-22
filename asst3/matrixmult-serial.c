#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* matrix functions */

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

matrix_t * matrix_create(int rows, int cols)
{
    matrix_t * this = malloc(sizeof(matrix_t));
    
    this->rows = rows,
    this->cols = cols,
    this->data = calloc(sizeof(double), this->rows * this->cols);
    
    return this;
}

void matrix_destroy(matrix_t * this)
{
    free(this->data);
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
        
        printf("%lf, ", matrix_idx(this, i, j));
        
        if (j == this->cols - 1) {
            printf("\n");
        }      
    }
    
    puts(")");
}

void matrix_mult_by_type(matrix_t ** C, matrix_t * A, matrix_t * B, matrix_mult_t type)
{
    int i, j, k, n = A->rows;

    assert(A);assert(B);assert(C);assert(*C);
    
    /* with the help of http://www.daniweb.com/software-development/csharp/code/355645/optimizing-matrix-multiplication */
    
    switch (type)
    {
        case MATRIX_MULT_IJK:
            for (i = 0; i < n; i++)
            {
                for (j = 0; j < n; j++)
                {
                    for (k = 0; k < n; k++) {
                        matrix_idx(*C, i, j) += matrix_idx(A, i, k) * matrix_idx(B, k, j);
                    }
                }
                printf("  finished row %d\n", i);
            }
            break;
        case MATRIX_MULT_IKJ:
            for (i = 0; i < n; i++)
            {
                for (k = 0; k < n; k++)
                {
                    for (j = 0; j < n; j++) {
                        matrix_idx(*C, i, j) += matrix_idx(A, i, k) * matrix_idx(B, k, j);
                    }
                }
            }
            break;
        case MATRIX_MULT_KIJ:
            for (k = 0; k < n; k++)
            {
                for (i = 0; i < n; i++)
                {
                    for (j = 0; j < n; j++) {
                        matrix_idx(*C, i, j) += matrix_idx(A, i, k) * matrix_idx(B, k, j);
                    }
                }
            }
            break;
    }
    
}

void get_and_fill_matrix_from_user_input(matrix_mult_t * mult_type, matrix_t ** A, matrix_t ** B)
{
    /* input must be in the form of
       <form>
       <n>
       <A>
       <B>
       
       <form> = ijk, ikj, kij
       <n> = integer*/
    enum {
        SCAN_FORM,
        SCAN_N,
        SCAN_A,
        SCAN_B,
        SCAN_END,
    } scan_state = SCAN_FORM;
    int i = 0, j = 0, n = 0;
    char form_buf[4] = {0};
    
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
                j = n = n * n;
                scan_state = SCAN_A;
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

int main(int argc, char * argv[])
{
    matrix_mult_t mult_type;
    matrix_t * A,
             * B,
             * C;
    get_and_fill_matrix_from_user_input(&mult_type, &A, &B);

    puts("received user input");

    C = matrix_create(A->rows, A->cols);

    matrix_mult_by_type(&C, A, B, mult_type);

    matrix_print(A);
    matrix_print(B);
    matrix_print(C);

    matrix_destroy(A);
    matrix_destroy(B);
    matrix_destroy(C);
    return 0;
}
