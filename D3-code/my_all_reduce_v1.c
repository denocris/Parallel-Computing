#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

#include "parameters.inc"

double seconds(){

    struct timeval tmp;
    double sec;
    gettimeofday( &tmp, (struct timezone *)0 );
    sec = tmp.tv_sec + ((double)tmp.tv_usec)/1000000.0;
    return sec;
}


int main( int argc, char *argv[] ){

  MPI_Request request;
  int rank, nprocs, prec, next;

  int i = 0, count = 0;
  size_t size_in_bytes;
  double *vector, *vector_total_manual_allreduce, *vector_total_allreduce, *buf_send, *buf_recv;
  double t_start, t_mine, t_manual_slowest, t_allreduce_slowest;
  
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &nprocs );
  
  next = ( rank + 1 ) % nprocs;
  prec = ( rank + nprocs - 1 ) % nprocs;

  size_in_bytes = SIZE * sizeof(double);

  vector = (double *) malloc( size_in_bytes );
  vector_total_manual_allreduce = (double *) malloc( size_in_bytes );
  vector_total_allreduce = (double *) malloc( size_in_bytes );
  buf_send = (double *) malloc( size_in_bytes );
  buf_recv = (double *) malloc( size_in_bytes );


  for( i = 0; i < SIZE; i++ ) vector[i] = (double) rank;
  memset( vector_total_manual_allreduce, 0, size_in_bytes );
  memset( vector_total_allreduce, 0, size_in_bytes );
  
  t_start = seconds();

  memcpy( buf_send, vector, size_in_bytes );
  memcpy( vector_total_manual_allreduce, vector, size_in_bytes );
  
  for( count = 1; count < nprocs; count++ ){

    MPI_Isend( buf_send, SIZE, MPI_DOUBLE, next, rank, MPI_COMM_WORLD, &request );
    MPI_Recv( buf_recv, SIZE, MPI_DOUBLE, prec, prec, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

    MPI_Wait( &request, MPI_STATUS_IGNORE );

    for( i = 0; i < SIZE; i++ ) vector_total_manual_allreduce[i] += buf_recv[i];

    memcpy( buf_send, buf_recv, size_in_bytes );

  }
  t_mine = seconds() - t_start;

  MPI_Reduce( &t_mine, &t_manual_slowest, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );

  t_start = seconds();
  MPI_Allreduce( vector, vector_total_allreduce, SIZE, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
  t_mine = seconds() - t_start;

  MPI_Reduce( &t_mine, &t_allreduce_slowest, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );

  if( !rank ){
    fprintf( stdout, "\n\tTime Manual Allreduce of %d DP elements = %.3g", SIZE, t_manual_slowest);
    fprintf( stdout, "\n\tTime Allreduce of %d DP elements = %.3g", SIZE, t_allreduce_slowest);
    fprintf( stdout, "\n\tCheck vector_total_manual_allreduce[1000] = %.3g", vector_total_manual_allreduce[1000] );
    fprintf( stdout, "\n\tCheck vector_total_allreduce[1000] = %.3g\n", vector_total_allreduce[1000] );
  }

  free( vector);
  free( buf_send );
  free( buf_recv );
  free( vector_total_manual_allreduce );
  free( vector_total_allreduce );
  
  MPI_Finalize();

  return 0;
}
  
