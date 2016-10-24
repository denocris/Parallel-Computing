#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#define SIZE 10


int main(int argc, char *argv[]){

  int rank, size;
  int i,r;
  MPI_Status status;
  double * A;

  MPI_Init( &argc, &argv);
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  MPI_Comm_size( MPI_COMM_WORLD, &size);

  A = (double*)malloc(SIZE*sizeof(double));

  for( i = 0; i < SIZE; i++) A[i] = rank;

  fprintf(stdout, "\nI am %d. Before point-2-point A[0]=%.3g \n", rank, A[0]);


  for( r = 0; r < size; r++){
    int curr = r;
    int next = (r+1)%size;

    if( rank == curr ){
    MPI_Send( A, SIZE, MPI_DOUBLE, next, 0, MPI_COMM_WORLD);
    }
    else if( rank == next ){
    MPI_Recv( A, SIZE, MPI_DOUBLE, curr, 0, MPI_COMM_WORLD, &status);
    fprintf(stdout, "\nI am %d. After %d point-2-point A[0]=%.3g \n", rank, r, A[0]);
    }


  }

  fprintf(stdout, "\nI am %d. After everything point-2-point A[0]=%.3g \n", rank, A[0]);

  MPI_Finalize();
  return 0;

}
