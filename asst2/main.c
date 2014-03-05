#include <stdio.h>
#include <math.h>

double calc_f_of_x(int x)
{
    return cos(x/3) - 2 * cos(x/5) + 5 * sin(x / 4) + 8;
}

int main()
{
    int i;

    puts("main");

    for (i = 0; i < 100; i += 5)
    {
        printf("f of %d = %lf\n", i, calc_f_of_x(i));
    }
    
    puts("done");

    return 0;
}
