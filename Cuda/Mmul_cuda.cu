/**
 * Vector reverse: A[i] = B[SIZE - i].
 *
 */

#include <stdio.h>
#include <cuda_runtime.h>

// SIZE is defined to be multiple of the number of threads
#define SIZE 4
#define THREADS_PER_BLOCK 2


__global__ void mat_mul( int *A, int *B, int *C, int size)
{

    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    int tmp_sum = 0;
    int k;

    if (row < size && col < size) {
    for (k = 0; k < size; k++)
    {
    tmp_sum  += A[row * size + k] * B[k * size + col];
    }
    //row+= gridDim.x * blockDim.x;
  }
    C[row * size + col] = tmp_sum;
}



int main( int argc, char * argv[])
{

  int i, j;
  int size_in_bytes = SIZE * SIZE * sizeof(int);
  int *dev_A;
  int *dev_B;
  int *dev_C;

  // Allocate the host input vector A
  int *host_A = (int *) malloc( size_in_bytes );
  int *host_B = (int *) malloc( size_in_bytes );
  int *host_C = (int *) malloc( size_in_bytes );

  // Initialize the host input vectors
  for( i = 0; i < SIZE * SIZE; i++ ){

      host_A[ i ] = i;
      host_B[ i ] = i;
      host_C[ i ] = 0;
  }

  for( i = 0; i < SIZE; i++ )
  {
    for( j = 0; j < SIZE; j++ )
      printf("%d \t", host_A[ i*SIZE + j ]);

    printf("\n");
  }

  for( i = 0; i < SIZE; i++ )
  {
    for( j = 0; j < SIZE; j++ )
      printf("%d \t", host_B[ i*SIZE + j ]);

    printf("\n");
  }

  for( i = 0; i < SIZE; i++ )
  {
    for( j = 0; j < SIZE; j++ )
      printf("%d \t", host_C[ i*SIZE + j ]);

    printf("\n");
  }


  cudaMalloc((void **)&dev_A, size_in_bytes);
  cudaMalloc((void **)&dev_B, size_in_bytes);
  cudaMalloc((void **)&dev_C, size_in_bytes);



  // copy inputs to device
  cudaMemcpy(dev_A, host_A, size_in_bytes, cudaMemcpyHostToDevice);
  cudaMemcpy(dev_B, host_B, size_in_bytes, cudaMemcpyHostToDevice);

  int threadsPerBlock = THREADS_PER_BLOCK;
  int blocksPerGrid =( SIZE * SIZE ) / threadsPerBlock;
  mat_mul<<<blocksPerGrid, threadsPerBlock>>>(dev_A, dev_B, dev_C, SIZE);

  cudaMemcpy(host_C, dev_C, size_in_bytes, cudaMemcpyDeviceToHost);

  for( i = 0; i < SIZE; i++ )
  {
    for( j = 0; j < SIZE; j++ )
      printf("%d \t", host_C[ i*SIZE + j ]);

    printf("\n");
  }

  printf("-------------------------\n\n");


  cudaFree(dev_A);
  cudaFree(dev_B);
  cudaFree(dev_C);

  // Free host memory

  free(host_A);
  free(host_B);
  free(host_C);

  return 0;
}
