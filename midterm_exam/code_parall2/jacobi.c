#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include <mpi.h>
#include <assert.h>

#include "multinode_matprint.h"
void multinode_save_gnuplot( double * loc_M,  size_t dimension, size_t loc_dimension, int rank, int comm_size );
void evolve( double * matrix, double *matrix_new, size_t dimension, size_t loc_dimension );
double seconds( void );

int main(int argc, char* argv[])
{

  int rank, comm_size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);


  // timing variables
  double t_start, t_end;

  // indexes for loops
  size_t i, j, it;

  // initialize matrix
  double *matrix, *matrix_new, *tmp_matrix;

  size_t dimension = 0, iterations = 0;
  size_t byte_dimension = 0;

  // check on input parameters
  if(argc != 3) {
    fprintf(stderr,"\nwrong number of arguments [NO PEAK value]. Usage: ./a.out dim it\n");
    return 1;
  }

  dimension = atoi(argv[1]);
  iterations = atoi(argv[2]);

  size_t loc_dimension = dimension/comm_size;
  assert(dimension%comm_size==0);


  // print messages only from rank 0
  if (rank==0)
  {
    printf("matrix size = %zu\n", dimension);
    printf("number of iterations = %zu\n", iterations);
  }


  // allocate LOCAL matrices on all the ranks
  byte_dimension = sizeof(double) * ( dimension + 2 ) * ( loc_dimension + 2 );
  matrix = ( double* )malloc( byte_dimension );
  matrix_new = ( double* )malloc( byte_dimension );

  // set everything to ZERO
  memset( matrix, 0, byte_dimension );
  memset( matrix_new, 0, byte_dimension );

  // fill initial values which are equals for all LOCAL matrices
  for( i = 1; i <= loc_dimension; ++i )
    for( j = 1; j <= dimension; ++j )
      matrix[ ( i * ( dimension + 2 ) ) + j ] = 0.5;

  // set up border conditions which varies with the rank
  double increment = 100.0 / ( dimension + 1 );

  // first coll condition between the ranks
  const int OFF = loc_dimension * rank;
  for (i = 1; i < loc_dimension+1; ++i)
  {
    matrix[ i * ( dimension + 2 ) ] = (i+OFF) * increment;
    matrix_new[ i * ( dimension + 2 ) ] = (i+OFF) * increment;
  }

  // last row condition
  if (rank == comm_size-1)
    for( i=1; i <= dimension+1; ++i )
    {
      matrix[ ( ( loc_dimension + 1 ) * ( dimension + 2 ) ) + ( dimension + 1 - i ) ] = i * increment;
      matrix_new[ ( ( loc_dimension + 1 ) * ( dimension + 2 ) ) + ( dimension + 1 - i ) ] = i * increment;
    }

//  print_multinode_matrix( matrix, dimension+2, loc_dimension+2, comm_size, rank );


  const int prev = (rank-1 >= 0)? rank-1 : MPI_PROC_NULL;
  const int next = (rank+1 < comm_size)? rank+1 : MPI_PROC_NULL;

  const int b_size = (dimension+2);

  double * const UP_recv[] = { matrix, matrix_new };
  double * const UP_send[] = { matrix+(dimension+2), matrix_new+(dimension+2) };
  double * const DN_recv[] = { matrix+((dimension+2)*(loc_dimension+1)), matrix_new+((dimension+2)*(loc_dimension+1)) };
  double * const DN_send[] = { matrix+((dimension+2)*loc_dimension), matrix_new+((dimension+2)*loc_dimension) };

  // start algorithm
  t_start = seconds();
  for( it = 0; it < iterations; ++it )
  {

    // halo exchange
    const size_t tog = it%2;
    MPI_Sendrecv(UP_send[tog], b_size, MPI_DOUBLE, prev, 0,
                 DN_recv[tog], b_size, MPI_DOUBLE, next, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Sendrecv(DN_send[tog], b_size, MPI_DOUBLE, next, 0,
                 UP_recv[tog], b_size, MPI_DOUBLE, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // do COMPUTATION
    evolve( matrix, matrix_new, dimension, loc_dimension );

    // swap the pointers
    tmp_matrix = matrix;
    matrix = matrix_new;
    matrix_new = tmp_matrix;

  }
  t_end = seconds();

//  print_multinode_matrix( matrix, dimension+2, loc_dimension+2, comm_size, rank );

  double slowest_time, loc_e_time = t_end - t_start;
  MPI_Reduce(&loc_e_time, &slowest_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (rank==0) printf( "\nelapsed time = %f seconds\n", slowest_time );

  multinode_save_gnuplot( matrix, dimension, loc_dimension, rank, comm_size );

  MPI_Finalize();

  free( matrix );
  free( matrix_new );

  return 0;
}




void evolve( double * matrix,
             double *matrix_new,
             size_t dimension,
             size_t loc_dimension )
{

  size_t i , j;

  //This will be a row dominant program.
  for( i = 1 ; i <= loc_dimension; ++i )
    for( j = 1; j <= dimension; ++j )
      matrix_new[ ( i * ( dimension + 2 ) ) + j ] = ( 0.25 ) *
	( matrix[ ( ( i - 1 ) * ( dimension + 2 ) ) + j ] +
	  matrix[ ( i * ( dimension + 2 ) ) + ( j + 1 ) ] +
	  matrix[ ( ( i + 1 ) * ( dimension + 2 ) ) + j ] +
	  matrix[ ( i * ( dimension + 2 ) ) + ( j - 1 ) ] );

}





void multinode_save_gnuplot( double * loc_M,
                             size_t dimension,
                             size_t loc_dimension,
                             int rank,
                             int comm_size )
{

  const size_t buff_size = (loc_dimension+2)*(dimension+2);

  if(rank==0)
  {

    size_t i , j;
    const double h = 0.1;
    FILE *file;

    file = fopen( "solution.dat", "w" );

    // save rank 0 data
    for( i = 0; i < loc_dimension + 1; ++i )
      for( j = 0; j < dimension + 2; ++j )
       fprintf(file, "%f\t%f\t%f\n", h * i, h * j, loc_M[ ( i * ( dimension + 2 ) ) + j ] );

    // save other ranks data
    double * tmp_M = malloc(buff_size*sizeof(double));
    int r;
    for (r = 1; r < comm_size; r++)
    {
      MPI_Recv(tmp_M, buff_size, MPI_DOUBLE, r, 99,  MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      const size_t c = (r==comm_size-1)?2:1; // for the last proc include last row (border condition)

      for( i = 1; i < loc_dimension + c; ++i )
        for( j = 0; j < dimension + 2; ++j )
          fprintf(file, "%f\t%f\t%f\n", h * (i + loc_dimension*r ), h * j, tmp_M[ ( i * ( dimension + 2 ) ) + j ] );

    }

    free(tmp_M);
    fclose(file);

  } else MPI_Send(loc_M, buff_size, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);


}




// A Simple timer for measuring the walltime
double seconds(){

    struct timeval tmp;
    double sec;
    gettimeofday( &tmp, (struct timezone *)0 );
    sec = tmp.tv_sec + ((double)tmp.tv_usec)/1000000.0;
    return sec;
}
