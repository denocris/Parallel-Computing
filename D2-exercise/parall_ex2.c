#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#define MAT_SIZE 10


int main(int argc, char *argv[])
{

  int rank, size;
  int i, j, sender;

  MPI_Status status;
  MPI_Request request;

  MPI_Init( &argc, &argv);
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  MPI_Comm_size( MPI_COMM_WORLD, &size);

  int loc_rows = MAT_SIZE/size;
  int rest = MAT_SIZE % size;
  if( rank < rest)
    loc_rows += 1;


  double * A = calloc(loc_rows * MAT_SIZE, sizeof(double));

  int start_row = (MAT_SIZE/size)*rank;

  if(rank < rest)
    start_row += rank;
  else
    start_row += rest;

  for( i = 0; i < loc_rows; i++)
    A[(start_row + i) + MAT_SIZE * i] = 1;


  if( rank == 0 )
  {
      for(i = 0; i < loc_rows; i++)
      {
          for( j=0; j < MAT_SIZE; j++)
            printf("%f ", A[i*MAT_SIZE + j]);
          printf("\n");
      }
      printf("_________________________________\n");

      double * B = malloc(loc_rows * MAT_SIZE * sizeof(double));
      int rec_rows = MAT_SIZE/size + 1;
      int before_rest = 1;

      for(sender = 1; sender < size; sender++)
      {
      if( before_rest && (sender >= rest)){

          rec_rows -=1;
          before_rest = 0;
      }

      MPI_Recv(B, rec_rows * MAT_SIZE,MPI_DOUBLE, sender, sender, MPI_COMM_WORLD, &status);

      for( i = 0; i < rec_rows; i++)
      {
          for(j = 0; j < MAT_SIZE; j++)
              printf("%f ", B[i*MAT_SIZE + j]);
          printf("\n");
      }
    }

      free(B);

  }

  else
  MPI_Send(A, loc_rows*MAT_SIZE, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD);


  MPI_Finalize();
  return 0;

}
