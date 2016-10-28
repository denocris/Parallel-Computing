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

  //const int N = atoi(argv[1]);     // Global dimension
  const int N = 4;     // Global dimension
  const int n = N/comm_size; // Local dimension

  assert(N%comm_size == 0);

  // Allocation of the local matrix slice (n x N)

  int * loc_A = malloc(N*n*sizeof(int));
  int * loc_A_buff = malloc(N*n*sizeof(int));
  int * loc_A_gh = malloc((N*n + N)*sizeof(int));

  // Generation of the local slice


  int i, j;
  for(i = 0; i < N * n; i++)
  {
    loc_A[i] = rank ;
  }

  for(i = 0; i < N * n; i++)
  {
    loc_A_buff[i] = rank;
  }

  for(i = 0; i < N * n + N; i++)
  {
    loc_A_gh[i] = rank;
  }


  //print_multinode_matrix(loc_A_gh, N, n + 1, comm_size, rank);
  print_multinode_matrix(loc_A_buff, N, n, comm_size, rank);

  const int PREC = (rank + comm_size -1)%comm_size;
  const int NEXT = (rank + 1)%comm_size;



//  MPI_Sendrecv(loc_A, n * N, MPI_INT, PREC, rank , loc_A_buff, n * N,
  //     MPI_INT, NEXT, NEXT, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

  MPI_Sendrecv(loc_A, n * N, MPI_INT, PREC, rank , loc_A_buff, n * N,
       MPI_INT, NEXT, NEXT, MPI_COMM_WORLD, MPI_STATUS_IGNORE );


  //printf("loc_A ___________________________________________ \n");
  print_multinode_matrix(loc_A, N, n, comm_size, rank);

  //printf("loc_A_buff ___________________________________________ \n");
  print_multinode_matrix(loc_A_buff, N, n, comm_size, rank);



  if(rank == 0)
  {
    for(i = 0; i < N * n; i++)
      {
        loc_A_gh[i] = loc_A[i];
      }
    for(j = 0; j < N; j++)
      {
        loc_A_gh[N * n + j] = loc_A_buff[j];
      }
  }

  if(rank == 1)
  {
    for(i = 0; i < N; i++)
      {
        loc_A_gh[i] = loc_A_buff[i];
      }
    for(j = 0; j < N * n; j++)
      {
        loc_A_gh[N + j] = loc_A[j];
      }
  }


  print_multinode_matrix(loc_A_gh, N, n + 1, comm_size, rank);


  MPI_Finalize();

  free(loc_A);
  free(loc_A_buff);
  free(loc_A_gh);



  return 0;

}
