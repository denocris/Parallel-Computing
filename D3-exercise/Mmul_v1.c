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

  MPI_Allgather(&loc_B[0], 1, MPI_INT, recv_buff, 1, MPI_INT, MPI_COMM_WORLD);
  // qua serve & perchè metto le [] e ci butto dentro il valore
  // così inviamo a tutti il primo elemento di ogni riga di B (cioè la sua colonna) e la mettiamo dentro recv_buff

  for( i = 0; i < glob_size; i++)
  {
    loc_C[0] += loc_A[i] * recv_buff[i];
  }

  //if( rank == size - 1)
  if( rank == 0)
  {
    for( i = 0; i < size; i++)
      printf("%d\n", recv_buff[i]);
      printf("C[0]= %d\n", loc_C[0]);
  }


  MPI_Finalize();



  return 0;

}
