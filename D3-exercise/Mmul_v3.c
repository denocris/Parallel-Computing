#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<assert.h>

/*

  Square matrix matrix multiplication.

*/

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

    int i;
    for (i = 1; i < mpi_size; i++)
    {
      MPI_Recv(loc_slice, glob_size*loc_size, MPI_INT, i, 0,  MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      print_slice (loc_slice, glob_size, loc_size);
    }

    printf("\n");

  } else MPI_Send(loc_slice, glob_size*loc_size, MPI_INT, 0, 0, MPI_COMM_WORLD);

}  


int main(int argc, char * argv[])
{

  int rank, size;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const int N = 32;     // Global dimension
  const int n = N/size; // Local dimension

  assert(N%size == 0);

  // Allocation of the local matrix slice (n x N)

  int * loc_A = malloc(N*n*sizeof(int));
  int * loc_B = malloc(N*n*sizeof(int));
  int * loc_C = malloc(N*n*sizeof(int));

  // Generation of the local slice

  int i;
  for(i=0; i<N*n; i++) 
    loc_A[i] = loc_B[i] = loc_C[i] = rank+1;

  // multi-node matrix-matrix multiplication 

  int * recv_buff = malloc(N*sizeof(int));
  int * send_buff = malloc(n*sizeof(int));

  // cycle on all columns 
  int j, k; 
  for (k=0; k < n; k++)
    for (j=0; j < N; j++)
    {

      // fill send buffer 
      for(i = 0; i < n; i++)
        send_buff[i] = loc_B[N*i+j];  

      // ALLgather 
      MPI_Allgather(send_buff, n, MPI_INT, recv_buff, n, MPI_INT, MPI_COMM_WORLD);

      // use recv buffer
      loc_C[k*N + j] = 0;
      for(i = 0; i < N; i++)
        loc_C[k*N + j] += loc_A[k*N + i]*recv_buff[i];

    }

  // rank==0 print the matrix, rank!=0 send the matrix
  print_multinode_matrix(loc_C, N, n, size, rank);

  MPI_Finalize();

  free(loc_A);
  free(loc_B);
  free(loc_C);

  free(recv_buff);
  free(send_buff);

  return 0;

}

