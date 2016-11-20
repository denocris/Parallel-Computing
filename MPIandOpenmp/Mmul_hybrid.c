#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include <omp.h>
#include<assert.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

/*

  Square matrix matrix multiplication: Hybrid MPI & Openmp version

*/


double seconds(){

/* Return the second elapsed since Epoch (00:00:00 UTC, January 1, 1970) */
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
    print_slice(loc_slice, glob_size, loc_size);

    int sender;
    for (sender = 1; sender < mpi_size; sender++)
    {
      MPI_Recv(loc_slice, glob_size*loc_size, MPI_INT, sender, 0,  MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      print_slice (loc_slice, glob_size, loc_size);
    }

    printf("\n");

  } else MPI_Send(loc_slice, glob_size*loc_size, MPI_INT, 0, 0, MPI_COMM_WORLD);

}


int main(int argc, char * argv[])
{

  int rank, size;

  double t_start;
  double t_sol;
  double t_comm_start;
  double t_comm = 0;
  double t_sol_slowest = 0;
  double t_comm_slowest = 0;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const int N = atoi(argv[1]);     // Global dimension
  const int n = N/size; // Local dimension

  assert(N%size == 0);

  // Allocation of the local matrix slice (n x N)

  int * loc_A = malloc(N*n*sizeof(int));
  int * loc_B = malloc(N*n*sizeof(int));
  int * loc_C = malloc(N*n*sizeof(int));

  // Generation of the local slice

  int i;
  for(i=0; i<N*n; i++)
  {
    loc_A[i] = loc_B[i] = rank + 1;
    loc_C[i] = 0;
  }

  // multi-node matrix-matrix multiplication

  int * recv_buff = malloc(N*sizeof(int));
  int * send_buff = malloc(n*sizeof(int));

  // cycle on all columns



  int j, k;

  t_start = seconds();

  //#pragma omp parallel for private(k,j,i)
  for (k=0; k < n; k++){
    #pragma omp parallel for private(j)
    for (j=0; j < N; j++)
    {

      // fill send buffer
      for(i = 0; i < n; i++)
        send_buff[i] = loc_B[N*i+j];


      // ALLgather
      t_comm_start = seconds();
      MPI_Allgather(send_buff, n, MPI_INT, recv_buff, n, MPI_INT, MPI_COMM_WORLD);
      t_comm += seconds() - t_comm_start;

      // use recv buffer
      loc_C[k*N + j] = 0;


      //t_start = seconds();

      //#pragma omp parallel for private(i)
      for(i = 0; i < N; i++)
        loc_C[k*N + j] += loc_A[k*N + i] * recv_buff[i];

    }
  }


  t_sol = seconds() - t_start;

  MPI_Reduce( &t_sol, &t_sol_slowest, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  MPI_Reduce( &t_comm, &t_comm_slowest, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);


  if( rank==0 )
  {
    fprintf( stdout, "152\t %.3g \t %.3g \t %.3g \t", t_sol_slowest, t_comm_slowest, t_sol_slowest - t_comm_slowest );
  }

  //print_multinode_matrix(loc_A, N, n, size, rank);
  //print_multinode_matrix(loc_B, N, n, size, rank);
  //print_multinode_matrix(loc_C, N, n, size, rank);

  MPI_Finalize();


  free(loc_A);
  free(loc_B);
  free(loc_C);

  free(recv_buff);
  free(send_buff);

  return 0;

}
