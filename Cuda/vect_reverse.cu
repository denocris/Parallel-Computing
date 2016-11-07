/**
 * Vector reverse: A[i] = B[SIZE - i].
 *
 */

#include <stdio.h>
#include <cuda_runtime.h>

// SIZE is defined to be multiple of the number of threads
#define SIZE 8
#define THREADS_PER_BLOCK 2


__global__ void vectorRev( float *A, float *B)
{
    int index = blockDim.x * blockIdx.x + threadIdx.x;

    A[ index ] = B[ SIZE - index ];
}



int main( int argc, char * argv[])
{

  int i;
  size_t size_in_bytes = SIZE * sizeof(int);

  // Allocate the host input vector A
  int *host_A = (int *) malloc( size_in_bytes );
  int *host_B = (int *) malloc( size_in_bytes );

  int *dev_A = cudaMalloc((void **)&dev_A, size_in_bytes);
  int *dev_B = cudaMalloc((void **)&dev_B, size_in_bytes);

  // Initialize the host input vectors
  for( i = 0; i < SIZE; i++ ){

      host_A[ i ] = i;
      host_B[ i ] = 0;
  }

  for( i = 0; i < SIZE; ++i )
    printf("%d", host_A[ i ]);

  for( i = 0; i < SIZE; ++i )
    printf("%d", host_B[ i ]);


  // copy inputs to device
  cudaMemcpy(dev_A, host_A, size_in_bytes, cudaMemcpyHostToDevice);
  cudaMemcpy(dev_B, host_B, size_in_bytes, cudaMemcpyHostToDevice);

  int threadsPerBlock = THREADS_PER_BLOCK;
  int blocksPerGrid =( SIZE ) / threadsPerBlock;
  vectorRev<<<blocksPerGrid, threadsPerBlock>>>(dev_A, dev_B);

  cudaMemcpy(host_B, dev_B, size_in_bytes, cudaMemcpyDeviceToHost);

  for( i = 0; i < SIZE; ++i )
    printf("%d", host_A[ i ]);

  for( i = 0; i < SIZE; ++i )
    printf("%d", host_B[ i ]);


  cudaFree(dev_A);
  cudaFree(dev_B);

  Free(host_A);
  Free(host_B);

  printf("Done\n");
  return 0;
}
