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


  for( i = 0; i < size; i++) A[i] = rank;

  A = (int*)malloc(size*sizeof(int));



  for( r = 1; r < size; r++){

    if( rank == r ){
    MPI_Send( &rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    else if( rank == 0 ){
    MPI_Recv( &A[r], 1, MPI_INT, r, 0, MPI_COMM_WORLD, &status);
   }

  }

//   if( rank ==0 ){
//   fprintf(stdout, "\nI am %d. After everything point-2-point A[0]=%d \n", rank, A[0]);
//   fprintf(stdout, "\nI am %d. After everything point-2-point A[1]=%d \n", rank, A[1]);
//   fprintf(stdout, "\nI am %d. After everything point-2-point A[2]=%d \n", rank, A[2]);
//   fprintf(stdout, "\nI am %d. After everything point-2-point A[3]=%d \n", rank, A[3]);
// }


  MPI_Finalize();
  return 0;

}
