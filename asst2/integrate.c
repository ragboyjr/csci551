/*
 * Majority of the MPI code was pulled from the pacheco source code
 * found here: http://www.ecst.csuchico.edu/~judyc/1314S-csci551/code/Pacheco/ch3/mpi_trap4.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "asst.h"

typedef struct {
    double a, b;
    int n;
} trap_data_t;

/* forward declartions */
static void build_mpi_type(trap_data_t * trap_data, MPI_Datatype * input_mpi_t);
static void get_user_input(int my_rank, int comm_sz, trap_data_t * trap_data);
static void calculate_local_input(int my_rank, int comm_sz, trap_data_t * global, trap_data_t * local);
static void print_statistics(double sum, double elapsed, trap_data_t * global, asst_t * as);

int main()
{
    int my_rank, comm_sz;
    double local_int,
           total_int,
           t_start,
           t_finish,
           t_elapsed;

    /* create the assignment object */
    asst_t * as = asst_create();
    
    trap_data_t global_input,
                local_input;

    /* Let the system do what it needs to start up MPI */
    MPI_Init(NULL, NULL);

    /* Get my process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out how many processes are being used */
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* fill the global_input with the user input */
    get_user_input(my_rank, comm_sz, &global_input);

    /* finished with io, let's start the timer */
    MPI_Barrier(MPI_COMM_WORLD);
    
    /* we only time rank 0 */
    if (my_rank == 0) {
        t_start = MPI_Wtime();
    }

    /* fill out the local input */
    calculate_local_input(my_rank, comm_sz, &global_input, &local_input);

    /* perform the local calculation */
    local_int = asst_trapezoidal_estimate(as, local_input.a, local_input.b, local_input.n);

    /* Add up the integrals calculated by each process */
    MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    
    /* Print the result */
    if (my_rank == 0) {
        t_finish  = MPI_Wtime(),
        t_elapsed = t_finish - t_start;
        
        print_statistics(total_int, t_elapsed, &global_input, as);
    }

    /* Shut down MPI */
    MPI_Finalize();
    
    /* free the asst object */
    asst_destroy(as);

    return 0;
} /*    main    */

/*------------------------------------------------------------------
 * Function: Build_mpi_type
 * Purpose: Build a derived datatype so that the three
 *          input values can be sent in a single message.
 * Input args:   trap_data: pointer to the input data
 * Output args:  input_mpi_t_p:  the new MPI datatype
 */
static void build_mpi_type(trap_data_t * trap_data, MPI_Datatype * input_mpi_t_p)
{
    int array_of_blocklengths[3] = {1, 1, 1};
    MPI_Datatype array_of_types[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
    MPI_Aint a_addr, b_addr, n_addr;
    MPI_Aint array_of_displacements[3] = {0};

    MPI_Get_address(&trap_data->a, &a_addr);
    MPI_Get_address(&trap_data->b, &b_addr);
    MPI_Get_address(&trap_data->n, &n_addr);
    array_of_displacements[1] = b_addr-a_addr; 
    array_of_displacements[2] = n_addr-a_addr; 
    MPI_Type_create_struct(
        3,
        array_of_blocklengths, 
        array_of_displacements, array_of_types,
        input_mpi_t_p
    );
    MPI_Type_commit(input_mpi_t_p);
}

/*------------------------------------------------------------------
 * Function:         Get_input
 * Purpose:          Get the user input:    the left and right endpoints
 *                           and the number of trapezoids
 * Input args:   my_rank:    process rank in MPI_COMM_WORLD
 *                           comm_sz:    number of processes in MPI_COMM_WORLD
 * Output args:  trap_data: pointer to the trapezoidal estimate data
 */
static void get_user_input(int my_rank, int comm_sz, trap_data_t * trap_data)
{
    MPI_Datatype input_mpi_t;
    int num_scanned;

    build_mpi_type(trap_data, &input_mpi_t);
    
    if (my_rank == 0)
    {
        puts("Enter a, b, and n");
        num_scanned = scanf("%lf %lf %d", &trap_data->a, &trap_data->b, &trap_data->n);
        
        if (num_scanned < 3)
        {
            puts("Invalid input given - defaulting arguments");
            trap_data->a = 0;
            trap_data->b = 1;
            trap_data->n = 1;
        }
    }
    
    /* broadcast the data */
    MPI_Bcast(&trap_data->a, 1, input_mpi_t, 0, MPI_COMM_WORLD);
    MPI_Type_free(&input_mpi_t);
}

/*------------------------------------------------------------------
 * Function: calculate_local_input
 * Purpose: calculates the local input for each process from the processes
 *          rank and the global input
 *
 * Input args: my_rank, comm_sz, global
 * Output args: local
 */
static void calculate_local_input(int my_rank, int comm_sz, trap_data_t * global, trap_data_t * local)
{
    double h = trapezoidal_calc_h(global->a, global->b, global->n);
    
    local->n = global->n / comm_sz;                /* problem size equally divided up */
    local->a = global->a + my_rank * local->n * h; /* leftmost local interval */
    local->b = local->a + local->n * h;            /* rightmost local interval */
}

/*
 * Print out the final statistics of the integration.
 */
static void print_statistics(double sum, double elapsed, trap_data_t * global, asst_t * as)
{
    int comm_sz;
    const char * fmt,
               * true_error_str = "";
    double abs_t_error;
    
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    fmt = 
        "\nRunning on %d processor(s).\n"
        "Elapsed time = %.7e seconds\n"
        "With n = %d trapezoid(s),\n"
        "our estimate of the integral from %lf to %lf = %.14e\n"
        "absolute relative true error = %e is%s less than criteria = %e\n";
    
    abs_t_error = asst_abs_true_error(as, sum);
    
    if (abs_t_error >= ASST_MIN_APPROX_VALUE) {
        true_error_str = " NOT";
    }
    
    printf(
        fmt,
        comm_sz,
        elapsed,
        global->n,
        global->a, global->b, sum,
        abs_t_error, true_error_str, ASST_MIN_APPROX_VALUE
    );
}
