#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>


int main(int argc, char *argv[])
{

  int rank, size;
  int i, j, sender;

  MPI_Status status;
  MPI_Request request;

  MPI_Init( &argc, &argv);
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  MPI_Comm_size( MPI_COMM_WORLD, &size);

  int to_send = rank;

  int * recv_buff = malloc(size * sizeof(int));



  MPI_Allgather(&to_send, 1, MPI_INT, recv_buff, 1, MPI_INT, MPI_COMM_WORLD);

  

  //if( rank == size - 1)
  if( rank == 0)
  {
    for( i = 0; i < size; i++)
      printf("%d\n", recv_buff[i]);
  }


  MPI_Finalize();



  return 0;

}
