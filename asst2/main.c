#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define MIN_APPROX_ERROR 5.0e-13

static double trapazoid_calc(double a, double b, long n, double (*func)(double))
{
    double h = (b - a) / (n),
           inner_sum = 0.0;
    int i;
           
    /* first calculate the inner sum */
    for (i = 1; i <= n - 1; i++) {
        inner_sum += func(a + i * h);
    }
    
    /* now just apply the formula and return */
    return h / 2 * (func(a) + 2 * inner_sum + func(b));
}

static double calc_f_of_x(double x)
{
    return cos(x/3) - 2 * cos(x/5) + 5 * sin(x / 4) + 8;
}

static double calc_actual_value()
{
    return 4000.0
    + 10.0 * sin(20)
    - 3.0 * sin(100.0/3.0)
    - 10.0 * sin(120)
    + 3.0 * sin(200)
    + 20.0 * cos(25)
    - 20.0 * cos(150);
}

static void find_n_binary()
{
    long min_value = 1,
         max_value = 100,
         num_traps = 0,
         i;
    double t_val, approx_val, t_error, abs_t_error;
    
    t_val = calc_actual_value();
    
    while (1)
    {
        /* get the absolute true error */
        approx_val  = trapazoid_calc(100, 600, max_value, calc_f_of_x),
        t_error     = t_val - approx_val,
        abs_t_error = cabs(t_error / t_val);
        
        /* have we converged to a value? */
        if (max_value - min_value <= 1) {
            num_traps = max_value;
            break;
        }
        
        if (abs_t_error >= MIN_APPROX_ERROR) {
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
    printf("actual value = %.14lg\n", t_val);
    printf("trap calc    = %.14lg\n", approx_val);
    printf("true error   = %.14lg\n", t_error);
    printf("abs t error  = %.14lg\n", abs_t_error);
    printf("min error    = %.14lg\n", MIN_APPROX_ERROR);
}

static void find_n(int argc, char * argv[])
{
    long num_traps;
    double t_val, approx_val, t_error, abs_t_error;
    
    if (argc < 2) {
        printf("usage: %s num_traps\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    t_val     = calc_actual_value(),
    num_traps = atof(argv[1]);
    
    approx_val  = trapazoid_calc(100, 600, num_traps, calc_f_of_x),
    t_error     = t_val - approx_val,
    abs_t_error = cabs(t_error / t_val);
    
    printf("t            = %ld\n", num_traps);
    printf("actual value = %.14lg\n", t_val);
    printf("trap calc    = %.14lg\n", approx_val);
    printf("true error   = %.14lg\n", t_error);
    printf("abs t error  = %.14lg\n", abs_t_error);
    printf("min error    = %.14lg\n", MIN_APPROX_ERROR);
}

int main(int argc, char * argv[])
{
    puts("main");

    if (argc < 2) {
        find_n_binary();
    }
    else {
        find_n(argc, argv);
    }

    puts("done");

    return 0;
}
