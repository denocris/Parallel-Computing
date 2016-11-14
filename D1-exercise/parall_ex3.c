#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
//#define SIZE 4


int main(int argc, char *argv[]){

  int rank, size;
  int i,r;
  MPI_Status status;
  int * A;

  MPI_Init( &argc, &argv);
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  MPI_Comm_size( MPI_COMM_WORLD, &size);


  A = (int*)malloc(size*sizeof(int));

  for( i = 0; i < size; i++) A[i] = rank;

  //fprintf(stdout, "\nI am %d. Before point-2-point A[0]=%d \n", rank, A[0]);

  for( r = 1; r < size; r++){

    if( rank == r ){
    MPI_Send( &rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    else if( rank == 0 ){
    MPI_Recv( &A[r], 1, MPI_INT, r, 0, MPI_COMM_WORLD, &status);
   }

  }

  if( rank ==0 )
  {
    for( i = 0; i < size; i++)
   fprintf(stdout, "\nI am %d. After point-2-point A[%d]=%d \n", rank, rank, A[i]);
  }


  MPI_Finalize();

  free(A);

  return 0;

}
