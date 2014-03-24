#include "asst.h"

#include <stdlib.h>
#include <math.h>
#include <complex.h>

static double func_in_integral(double);

/*
 * allocated and initialize the assignment struct with the default
 * values and return it
 */
asst_t * asst_create()
{
    asst_t * this = malloc(sizeof(asst_t));
    
    this->true_value = ASST_TRUE_VALUE;
    this->f          = func_in_integral;
    
    return this;
}
/* destroy the assignment struct */
void asst_destroy(asst_t * this)
{
    free(this);
}

double asst_trapezoidal_estimate(asst_t * this, double a, double b, double n)
{
    double h = trapezoidal_calc_h(a, b, n),
           inner_sum = 0.0;
    int i;
           
    /* first calculate the inner sum */
    for (i = 1; i <= n - 1; i++) {
        inner_sum += this->f(a + i * h);
    }
    
    /* now just apply the formula and return */
    return h / 2 * (this->f(a) + 2 * inner_sum + this->f(b));
}

/*
 * calculate f(x) for the function given in the 
 * assignment instructions
 */
static double func_in_integral(double x)
{
    return cos(x/3) - 2 * cos(x/5) + 5 * sin(x / 4) + 8;
}
