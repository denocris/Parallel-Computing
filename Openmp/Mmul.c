#include<stdio.h>
#include<stdlib.h>
#include <omp.h>
#include<assert.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

/*

  Square matrix matrix multiplication.

*/


double seconds(){

/* Return the second elapsed since Epoch (00:00:00 UTC, January 1, 1970) */
  struct timeval tmp;
  double sec;
  gettimeofday( &tmp, (struct timezone *)0 );
  sec = tmp.tv_sec + ((double)tmp.tv_usec)/1000000.0;
  return sec;
}

void MatMul_ser(int * A, int * B, int * C, int N)
{

  int i, j, k;

  double time_start, time_stop;

  // Generation of the local slice

  time_start = seconds();

  for(i = 0; i < N; i++)
    for(j = 0; j < N; j++)
      for(k = 0; k < N; k++)
      {
        C[i * N + j] += A[i * N + k] * B[k * N + j];
      }

  time_stop = seconds() - time_start;

  printf("%f\n", time_stop);

}



void MatMul_omp(int * A, int * B, int * C, int N)
{

  int i, j, k;

  int idx = 0;
  int num_thrs = 0;
  double time_start, time_stop;

  // Generation of the local slice

  time_start = seconds();


  #pragma omp parallel private(idx, num_thrs)
  {

    idx = omp_get_thread_num();
    num_thrs = omp_get_num_threads();


    #pragma omp for private(i, j, k)
    for(i = 0; i < N; i++)
      for(j = 0; j < N; j++)
        for(k = 0; k < N; k++)
        {
          C[i * N + j] += A[i * N + k] * B[k * N + j];
        }

  }

  time_stop = seconds() - time_start;

  printf("%f\n", time_stop);

}

int main(int argc, char * argv[]){

  int N = atoi(argv[1]);
  int i, j;


  // Allocation of the local matrix slice (N x N)

  int * A = malloc(N*N*sizeof(int));
  int * B = malloc(N*N*sizeof(int));
  int * C = malloc(N*N*sizeof(int));

  for(i = 0; i < N; i++)
    for(j = 0; j < N; j++)
    {
      A[i * N + j] = i + j;
      B[i * N + j] = i - j;
    }

// here we compute the matrix multiplication A * B = C in a serial way
  MatMul_ser(A,B,C,N);

// here we compute the matrix multiplication A * B = C using Openmp
  MatMul_omp(A,B,C,N);

/*
  printf("---------------------------\n");


    for(i = 0; i < N; i++)
    {
      for(j = 0; j < N; j++)
      {
        printf("%d\t", A[i * N + j] );
      }
      printf("\n");
    }


  printf("---------------------------\n");

    for(i = 0; i < N; i++)
    {
      for(j = 0; j < N; j++)
      {
        printf("%d\t", B[i * N + j] );
      }
      printf("\n");
    }

  printf("---------------------------\n");

      for(i = 0; i < N; i++)
      {
        for(j = 0; j < N; j++)
        {
          printf("%d\t", C[i * N + j] );
        }
        printf("\n");
      }
*/

}
