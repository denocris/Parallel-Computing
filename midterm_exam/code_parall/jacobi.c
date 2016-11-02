#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

/*** function declarations ***/

// save matrix to file
//void save_gnuplot( double *M, size_t dim );




void print_slice (double * slice, int glob_size, int loc_size)
{

  int i, j;
  for(i=0; i < loc_size; i++)
  {
    for(j=0; j < glob_size; j++)
      printf(" %f", slice[glob_size*i + j]);
    printf("\n");
  }
      printf("\n");
}



void print_multinode_matrix( double * loc_slice,
                             int glob_size,
                             int loc_size,
                             int mpi_size,
                             int rank )
{

  if(rank == 0)
  {

    printf("\n");
    print_slice (loc_slice, glob_size, loc_size);

    double * tmp_slice = malloc(glob_size*loc_size*sizeof(double));
    int i;
    for (i = 1; i < mpi_size; i++)
    {
      MPI_Recv(tmp_slice, glob_size*loc_size, MPI_DOUBLE, i, 99,  MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      print_slice (tmp_slice, glob_size, loc_size);
    }

    printf("\n");
    free(tmp_slice);

  } else MPI_Send(loc_slice, glob_size*loc_size, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);

}





void gnuplot( double * loc_slice,
                             int glob_size,
                             int loc_size,
                             int mpi_size,
                             int rank )
{

  if(rank == 0)
  {

    FILE *file;
    file = fopen( "solution.dat", "w+" );
    const double h = 0.1;
    int i, j, r;

    double * tmp_slice = malloc(glob_size*(loc_size + 2)*sizeof(double));

    for(i=0; i <= loc_size; i++)
    {
      for(j=0; j < glob_size; j++)
      {
        printf(" %f", loc_slice[glob_size*i + j]);
        fprintf(file, "%f\t%f\t%f\n", h * (loc_size * r + i ), h * j, loc_slice[glob_size*i + j] );
      }
      printf("\n");

    }
      printf("\n");


    int sender;

    for(sender = 1; sender < mpi_size; sender++)
    {
      if( sender != mpi_size - 1 )
      {
        MPI_Recv(tmp_slice, glob_size*(loc_size + 2), MPI_DOUBLE, sender, 99,  MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(i = 1; i <= loc_size; i++)
        {
          for(j = 0; j < glob_size; j++)
          {
            printf(" %f", tmp_slice[glob_size*i + j]);
            fprintf(file, "%f\t%f\t%f\n", h * (loc_size * r + i ), h * j, tmp_slice[glob_size*i + j] );
          }
          printf("\n");
        }
      }
      else
      {
        MPI_Recv(tmp_slice, glob_size*(loc_size + 2), MPI_DOUBLE, sender, 99,  MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for(i = 1; i <= loc_size + 1; i++)
        {
          for(j=0; j < glob_size; j++)
          {
            printf(" %f", tmp_slice[glob_size*i + j]);
            fprintf(file, "%f\t%f\t%f\n", h * (loc_size * r + i ), h * j, tmp_slice[glob_size*i + j] );
          }
        printf("\n");

      }
    }

    printf("\n");


  }
  fclose( file );
 }
  else MPI_Send(loc_slice, glob_size*(loc_size + 2), MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
}


/*** end function declaration ***/

int main(int argc, char* argv[]){

  int rank, size;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Status status;
  MPI_Request request;



  size_t dimension = 0, iterations = 0, row_peek = 0, col_peek = 0;
  //size_t byte_dimension = 0;

  // timing variables
  double t_start, t_end, increment;

  // indexes for loops
  size_t i, j, it;

  // initialize matrix
  double *loc_matrix, *loc_matrix_new, *tmp_matrix;


  // check on input parameters
  if(argc != 5) {
    fprintf(stderr,"\nwrong number of arguments. Usage: ./a.out dim it n m\n");
    return 1;
  }

  dimension = atoi(argv[1]);
  iterations = atoi(argv[2]);
  row_peek = atoi(argv[3]);
  col_peek = atoi(argv[4]);

  const int N = dimension + 2;     // Global dimension
  const int n = dimension/size; // Local dimension


  printf("matrix size = %zu\n", dimension);
  printf("number of iterations = %zu\n", iterations);
  printf("element for checking = Mat[%zu,%zu]\n",row_peek, col_peek);

  if((row_peek > dimension) || (col_peek > dimension)){
    fprintf(stderr, "Cannot Peek a matrix element outside of the matrix dimension\n");
    fprintf(stderr, "Arguments n and m must be smaller than %zu\n", dimension);
    return 1;
  }


  //byte_dimension = sizeof(double*) * ( dimension + 2 ) * ( dimension + 2 );
  int loc_n_gh = n + 2;
  loc_matrix = ( double* )malloc( N * loc_n_gh *sizeof(double));
  loc_matrix_new = ( double* )malloc( N * loc_n_gh *sizeof(double));

  memset( loc_matrix, 0, N * loc_n_gh * sizeof(double) );
  memset( loc_matrix_new, 0, N * loc_n_gh * sizeof(double) );


  for( i = 1; i <= n; ++i )
    for( j = 1; j <= dimension ; ++j )
      loc_matrix[ i * N + j ] = 0.5;


  increment = 100.0 / ( dimension + 1 );

  for( i = 1; i <= n ; ++i )
  {
    loc_matrix[ i * N ] = (n*rank + i)* increment;
    loc_matrix_new[ i * N ] = (n*rank +i) * increment;
  }


  if( rank==size - 1 )
  for( i = 1; i < dimension + 2; ++i ){
    loc_matrix[ (n + 1) * N + ( dimension + 1 - i )] = i * increment;
    loc_matrix_new[(n + 1) * N + ( dimension + 1 - i )] = i * increment;
  }

  print_multinode_matrix(loc_matrix, N, loc_n_gh , size, rank);


  int PREC = (rank + size -1)%size;
  int NEXT = (rank + 1)%size;

  if( rank==0 ) PREC = MPI_PROC_NULL;
  if( rank== size -1 ) NEXT = MPI_PROC_NULL;


  MPI_Sendrecv(&loc_matrix[(loc_n_gh - 2)*N], N, MPI_DOUBLE, NEXT, 0, &loc_matrix[0*N], N,
       MPI_DOUBLE, PREC, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

  MPI_Sendrecv(&loc_matrix[1*N], N, MPI_DOUBLE, PREC, 1, &loc_matrix[(loc_n_gh - 1)*N], N,
            MPI_DOUBLE, NEXT, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE );


  print_multinode_matrix(loc_matrix, N, loc_n_gh , size, rank);

  gnuplot(loc_matrix, N, n , size, rank);
  //print_multinode_matrix2(loc_matrix, dimension, n , size, rank);

  MPI_Finalize();

  free( loc_matrix );
  free( loc_matrix_new );

  return 0;
}
