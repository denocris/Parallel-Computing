#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

/*

  Square matrix matrix multiplication.

*/

double seconds(){

    struct timeval tmp;
    double sec;
    gettimeofday( &tmp, (struct timezone *)0 );
    sec = tmp.tv_sec + ((double)tmp.tv_usec)/1000000.0;
    return sec;
}

void print_slice (int * slice, int glob_size, int loc_size)
{

  int i, j;
  for(i=0; i < loc_size; i++)
  {
    for(j=0; j < glob_size; j++)
      printf(" %d", slice[glob_size*i + j]);
    printf("\n");
  }

}


void print_multinode_matrix( int * loc_slice,
                             int glob_size,
                             int loc_size,
                             int mpi_size,
                             int rank )
{

  if(rank == 0)
  {

    printf("\n");
    print_slice (loc_slice, glob_size, loc_size);

    int * tmp_slice = malloc(glob_size*loc_size*sizeof(int));
    int i;
    for (i = 1; i < mpi_size; i++)
    {
      MPI_Recv(tmp_slice, glob_size*loc_size, MPI_INT, i, 99,  MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      print_slice (tmp_slice, glob_size, loc_size);
    }

    printf("\n");
    free(tmp_slice);

  } else MPI_Send(loc_slice, glob_size*loc_size, MPI_INT, 0, 99, MPI_COMM_WORLD);

}


int main(int argc, char * argv[])
{

  int rank, comm_size;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  MPI_Status status; // restituito dal Recv (bloccante)
  MPI_Request request; // va per le comunicazioni non bloccanti

  const int N = 4;     // Global dimension
  const int n = N/comm_size; // Local dimension

  assert(N%comm_size == 0);

  // Allocation of the local matrix slice (n x N)

  int * loc_A = malloc(N*n*sizeof(int));
  int * loc_B = malloc(N*n*sizeof(int));
  int * loc_C = malloc(N*n*sizeof(int));

  // Generation of the local slice

  int i, j, k;
  for(i = 0; i < N * n; i++)
  {
    loc_A[i] = rank + 1;
    loc_B[i] = rank + 1;
    loc_C[i] = 0;

  }

  const int NEXT = (rank + 1)%comm_size;
  const int PREC = (rank + comm_size -1)%comm_size;

  //print_multinode_matrix(loc_A, N, n, comm_size, rank);
  //print_multinode_matrix(loc_B, N, n, comm_size, rank);

  double t_start, t_end, t_slowest, t_start_communication, t_end_communication;

  t_start = seconds();

  int count, count2;

  for( count = 0; count < comm_size; count++)
    {

      int col_off_A = ((rank + count)*n)%N;

      for( count2 = 0; count2 < comm_size; count2++)
        {
          int col_off_B = (count2*n)%N;

          for( i = 0; i < n; i++)
            for( j = 0; j < n; j++)
              for(k = 0; k < n; k++)
              {
                loc_C[i*N + j + col_off_B] += loc_A[i*N + col_off_A + k] * loc_B[k*n + col_off_B + j];

                //print_multinode_matrix(loc_C, N, n, comm_size, rank);
              }
          }

      // devo fare un buffer di loc_B, altrimenti sovrascrive su loc_B
      //MPI_Isend(loc_B, n * N, MPI_INT, PREC, 0, MPI_COMM_WORLD, &request);
      //MPI_Recv(loc_B, n * N, MPI_INT, NEXT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      // Non serve il buffer dove scrivere
      t_start_communication = seconds();
      MPI_Sendrecv_replace( loc_B, n * N, MPI_INT, PREC, rank, NEXT, NEXT, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
      t_end_communication = seconds() - t_start_communication;
    }

  t_end = seconds() - t_start;

  MPI_Reduce( &t_end, &t_slowest, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );

  if( !rank ){ // !rank is the same of saying rank == 0
    //fprintf( stdout, "\tTime to sol of %.3g seconds for SIZE = %d \n\n", t_slowest, comm_size );
    //fprintf( stdout, "\tTime to com of %.3g seconds for SIZE = %d \n\n", t_end_communication, comm_size );
    printf("%.3g %.3g %d %d \n\n", t_slowest, t_end_communication, N, comm_size );
  }

  //print_multinode_matrix(loc_C, N, n, comm_size, rank);



  MPI_Finalize();

  free(loc_A);
  free(loc_B);
  free(loc_C);


  return 0;

}
