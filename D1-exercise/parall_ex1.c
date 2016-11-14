#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#define SIZE 10


int main(int argc, char *argv[]){

  int rank, size;
  int i;
  MPI_Status status;
  double * A;

  MPI_Init( &argc, &argv);
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  MPI_Comm_size( MPI_COMM_WORLD, &size);

  A = (double*)malloc(SIZE*sizeof(double));

  for( i = 0; i < SIZE; i++) A[i] = rank;


  fprintf(stdout, "\nI am %d. Before point-2-point A[0]=%.3g \n", rank, A[0]);

  if( rank==0 ){
  MPI_Send( A, SIZE, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
  }
  else {
  MPI_Recv( A, SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
  }

  fprintf(stdout, "\nI am %d. After point-2-point A[0]=%.3g \n", rank, A[0]);



  for( i = 0; i < SIZE; i++) A[i] = rank + 2;

  if( rank==1 ){
  MPI_Send( A, SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
  else {
  MPI_Recv( A, SIZE, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
  }

  fprintf(stdout, "\nI am %d. After another point-2-point A[0]=%.3g \n", rank, A[0]);


  MPI_Finalize();

  free(A);

  return 0;

}
