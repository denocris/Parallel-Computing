#ifndef __MULTINODE_MATPRINT_H__
#define __MULTINODE_MATPRINT_H__

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

void print_slice (double * slice, int glob_size, int loc_size)
{

  int i, j;
  for(i=0; i < loc_size; i++)
  {
    for(j=0; j < glob_size; j++)
      printf(" %f", slice[glob_size*i + j]);
    printf("\n");
  }    

  printf("\n");

}


void print_multinode_matrix( double * loc_slice,
                             int glob_size,
                             int loc_size,
                             int mpi_size,
                             int rank )
{

  if(rank == 0)
  {

    printf("\n");
    print_slice (loc_slice, glob_size, loc_size);


    double * tmp_slice = malloc(glob_size*loc_size*sizeof(double));
    int i;
    for (i = 1; i < mpi_size; i++)
    {
      MPI_Recv(tmp_slice, glob_size*loc_size, MPI_DOUBLE, i, 99,  MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      print_slice (tmp_slice, glob_size, loc_size);
    }

    printf("\n");

    free(tmp_slice);

  } else MPI_Send(loc_slice, glob_size*loc_size, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);

}  

#endif

