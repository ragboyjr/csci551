#include <stdio.h>
#include <stdlib.h>
#include "asst.h"

static void find_n_binary()
{
    long min_value = 1,
         max_value = 100,
         num_traps = 0,
         i;
    double approx_val,
           abs_t_error;
    asst_t * as = asst_create();
    
    while (1)
    {
        /* run the trapazoidal estimate */
        approx_val = asst_trapezoidal_estimate(as, 100, 600, max_value);
        
        /* get the absolute true error */
        abs_t_error = asst_abs_true_error(as, approx_val);
        
        /* have we converged to a value? */
        if (max_value - min_value <= 1) {
            num_traps = max_value;
            break;
        }
        
        if (abs_t_error >= ASST_MIN_APPROX_VALUE) {
            /* double the max value */
            min_value = max_value;
            max_value *= 2;
        }
        else {
            /* it's less than, use binary search */
            max_value = min_value + ((max_value - min_value) / 2);
        }
    }
    
    printf("t            = %ld\n", num_traps);
    printf("actual value = %.14lg\n", as->true_value);
    printf("trap calc    = %.14lg\n", approx_val);
    printf("true error   = %.14lg\n", as->true_value - approx_val);
    printf("abs t error  = %.14lg\n", abs_t_error);
    printf("min error    = %.14lg\n", ASST_MIN_APPROX_VALUE);
    
    asst_destroy(as);
}

static void find_n(long num_traps)
{
    double t_val, approx_val, t_error, abs_t_error;
    asst_t * as = asst_create();
    
    approx_val  = asst_trapezoidal_estimate(as, 100, 600, num_traps);
    abs_t_error = asst_abs_true_error(as, approx_val);
    
    printf("t            = %ld\n", num_traps);
    printf("actual value = %.15e\n", t_val);
    printf("trap calc    = %.15e\n", approx_val);
    printf("true error   = %.15e\n", t_error);
    printf("abs t error  = %.15e\n", abs_t_error);
    printf("min error    = %.15e\n", ASST_MIN_APPROX_VALUE);
    
    asst_destroy(as);
}

int main(int argc, char * argv[])
{
    puts("main");

    if (argc < 2) {
        find_n_binary();
    }
    else {
        find_n(atol(argv[1]));
    }

    puts("done");

    return 0;
}
