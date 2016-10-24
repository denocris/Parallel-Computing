#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>


int main(int argc, char *argv[])
{

  int rank, size;
  int i, j, k;

  MPI_Status status;
  MPI_Request request;

  MPI_Init( &argc, &argv);
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  MPI_Comm_size( MPI_COMM_WORLD, &size);

  const int glob_size = 3;
  const int loc_size = 1;

  int * loc_A = malloc(loc_size * glob_size * sizeof(int));
  int * loc_B = malloc(loc_size * glob_size * sizeof(int));
  int * loc_C = malloc(loc_size * glob_size * sizeof(int));


  for( i = 0; i < glob_size*loc_size; i++)
    {
      loc_A[i] = rank + 1;
      loc_B[i] = rank + 1;
      loc_C[i] = 0;
    }

  int * recv_buff = malloc(loc_size * glob_size * sizeof(int));

  for( j = 0; j < glob_size; j++)
    {
      MPI_Allgather(&loc_B[j], 1, MPI_INT, recv_buff, 1, MPI_INT, MPI_COMM_WORLD);
      for( i = 0; i < glob_size; i++)
      {
        loc_C[j] += loc_A[i] * recv_buff[i];
      }
    }

  for( j = 0; j < size; j++)
  {
    if( rank == j)
      {
        for( i = 0; i < size; i++)
          {
            //printf("**** %d\n", recv_buff[i]);
            printf("On rank %d, C[%d]= %d\n", j, i, loc_C[i]);
          }

      }
  }


  MPI_Finalize();

  free(loc_A);
  free(loc_B);
  free(loc_C);



  return 0;

}
