#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>


int main(int argc, char *argv[]){

  int rank, size;
  int i, num_steps, loc_sum, rec_rank;
  int NEXT, PREC;

  MPI_Status status;
  MPI_Request request[2];

  MPI_Init( &argc, &argv);
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  MPI_Comm_size( MPI_COMM_WORLD, &size);

  num_steps = size - 1;
  loc_sum = rank;

  int send_rank = rank;

  NEXT = (rank + 1) % size;
  PREC = (rank + size - 1) % size;

  for( i = 0; i < num_steps; i++){


    MPI_Isend( &send_rank, 1, MPI_INT, NEXT, 0, MPI_COMM_WORLD, &request[0]);

    MPI_Irecv( &rec_rank, 1, MPI_INT, PREC, 0, MPI_COMM_WORLD, &request[1]);

    MPI_Waitall(2, request, MPI_STATUS_IGNORE);

    loc_sum += rec_rank;

    //MPI_Waitall(2, request, MPI_STATUS_IGNORE);

    printf("\n I am %d. At step %d, local_sum is %d \n, send_rank is %d, rec_rank is %d", rank, i,
    loc_sum, send_rank, rec_rank);

    send_rank = rec_rank;


  }

  printf("\n I am %d. The number of steps is %d. The sum is %d \n", rank, num_steps, loc_sum);

  MPI_Finalize();
  return 0;

}
