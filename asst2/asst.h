#ifndef _ASST2_ASST_H
#define _ASST2_ASST_H

/*
 * Asst is the object responsible for the common functions for this assignment.
 * It holds the shared code for the find-n program and the integrate program.
 * It mainly just holds the code for running the integral and calculating absolute 
 * relative true error
 */
 
#include <complex.h>

#define ASST_TRUE_VALUE 4000.0\
    + 10.0 * sin(20)\
    - 3.0 * sin(100.0/3.0)\
    - 10.0 * sin(120)\
    + 3.0 * sin(200)\
    + 20.0 * cos(25)\
    - 20.0 * cos(150)
    
#define ASST_MIN_APPROX_VALUE 5.0e-13

#define trapezoidal_calc_h(a, b, n) ((b - a) / n)

typedef struct _asst_t {
    double true_value;
    double (*f)(double);
} asst_t;



asst_t * asst_create();
void     asst_destroy(asst_t *);

double asst_trapezoidal_estimate(asst_t *, double /* a */, double /* b */, double /* n */);

static inline double asst_abs_true_error(asst_t * this, double approx_val) {    
    return cabs((this->true_value - approx_val) / this->true_value);
}

#endif
