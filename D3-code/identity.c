/*
 * This code implements the parallel initializzation of an identity matrix. 
 * At the the matrix is written on a binary file by process 0 who receives the various sestion of the 
 * matrix by the other processes. The proposed parallel output is not efficient. 
 * It is implemented as solution of exerices given to practise with the MPI paradigm
 *
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define NSIZE 10

/* prints a section of matrix of size = NSIZE x n_loc */
void print_matrix( int * matrix, int n_loc ){

  int i, j;
  FILE * fp;

  /* 
   * open the file in write mode, creating the file if not existing
   * The stream is positioned at the end of the file 
   */
  fp = fopen( "matrix.dat", "a" );

  fwrite( matrix, n_loc * NSIZE, sizeof(int), fp );

  fclose( fp );

}

void print_parallel( int * matrix, int my_rank, int npes, int n_loc, int rest ){
  
  int count;

  if( !my_rank ){

    /* print matrix section of process 0 */
    print_matrix( matrix, n_loc );

    /* if rest!=0, process 0 re-allocate the buffer to contain larger sections of the matrix */
    if( rest ){
      
      free( matrix );
      matrix = (int *) malloc( (n_loc + 1) * NSIZE * sizeof(int) );
    }

    for( count = 1; count < npes; count ++ ){

      /* 
       * prepare to receive larger buffers if coming 
       * section of the matrix is expected to contain rests 
       */
      if( count == (npes - rest) ) n_loc = n_loc + 1;

      MPI_Recv( matrix, NSIZE * n_loc, MPI_INT, count, count, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
      print_matrix( matrix, n_loc);
    }
  }
  else {
     
    MPI_Send( matrix, NSIZE * n_loc, MPI_INT, 0, my_rank, MPI_COMM_WORLD );
  }
}
  

int main(int argc, char * argv[]){

  size_t buffer_size_bytes;
  int * identity;

  int rest, offset = 0; /* store variable to handle unbalanced data distribution */
  int n_loc; /* store local size of the matrix */
  int i_global, j_global; /*store local indexes */
  int i_loc, j_loc; /*store global indexes */

  /* MPI related variables */
  int my_rank, npes;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size( MPI_COMM_WORLD, &npes);
  MPI_Comm_rank( MPI_COMM_WORLD, &my_rank);

  /* compute the local dimensions */ 
  n_loc = NSIZE / npes;
  rest = NSIZE % npes;

  /* distribute rests in reverse order from the higher rank */  
  if( ( rest != 0 ) && ( my_rank >= ( npes - rest ) ) ){
    n_loc = n_loc + 1;
    offset = my_rank - ( npes - rest );
  }

  buffer_size_bytes = n_loc * NSIZE * sizeof(int);
  identity = (int *) malloc( buffer_size_bytes );

  /* initialization of all elements to 0 */
  memset( identity, 0, buffer_size_bytes );

  /* initialization at 1 of all elements along the diagonal of the identity matrix (global)  */
  for( i_loc = 0; i_loc < n_loc; i_loc++ ){

    /* trasforming indexes from local to global */
    i_global = i_loc + ( ( NSIZE / npes ) * my_rank) + offset;
    identity[ ( i_loc * NSIZE ) + i_global ] = 1;
  }

  /* print the matrix in parallel */
  print_parallel( identity, my_rank, npes, n_loc, rest );

  free( identity );

  MPI_Finalize();

  return 0;
}
  
