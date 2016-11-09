#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#include <cuda_runtime.h>

#define SIZE 2048
#define THREADS_PER_BLOCK 512

// A Simple timer for measuring the walltime
double seconds(){

    struct timeval tmp;
    double sec;
    gettimeofday( &tmp, (struct timezone *)0 );
    sec = tmp.tv_sec + ((double)tmp.tv_usec)/1000000.0;
    return sec;
}

// Multithreaded naive version of Matrix GEMM
void matmul_cpu( double *A, double *B, double *C ){

    int i, j, k;
    double loc_sum;

#pragma omp parallel for private( i, j, k, loc_sum )
    for(i = 0; i < SIZE; i++ ){
        for( j = 0; j < SIZE; j++ ){

	  loc_sum = 0.0;
	  for( k = 0; k < SIZE; k++ ){
	    loc_sum += A[ (SIZE * i) + k ] * B[ (SIZE * k) + j ];
	  }
	  C[ (SIZE * i) + j ] = loc_sum;
        }
    }
}

// GPU naive version of Matrix GEMM using shared memory
__global__ void CUDA_matmul_shared( double *A, double *B, double *C ){

  // Implement e version with shared memory

}


// GPU naive version of Matrix GEMM
__global__ void CUDA_matmul( double *A, double *B, double *C ){

  int i, j, k;
  double loc_sum;
  
  i = blockIdx.x;
  j = threadIdx.x;

  if( i < SIZE ){

    while( j < SIZE){ 
     
      loc_sum = 0.0;
      for( k = 0; k < SIZE; k++ ) loc_sum += A[ (SIZE * i) + k] * B[ ( SIZE * k ) + j];
      C[ (SIZE * i) + j ] = loc_sum;
      j += blockDim.x;
    }
  }
}

/* do matrix multiplication on the GPU here */
void matmul_gpu(double *A, double *B, double *C ){

    size_t size_in_bytes;
    cudaError_t err;
    double *d_A, *d_B, *d_C;

    size_in_bytes = sizeof(double) * SIZE * SIZE;
    cudaMalloc( (void **) &d_A, size_in_bytes);
    cudaMemcpy( d_A, A, size_in_bytes, cudaMemcpyHostToDevice );

    cudaMalloc( (void **) &d_B, size_in_bytes);
    cudaMemcpy( d_B, B, size_in_bytes, cudaMemcpyHostToDevice );

    cudaMalloc((void **) &d_C, size_in_bytes);

#ifdef __SHARED
    CUDA_matmul_shared<<< SIZE, THREADS_PER_BLOCK >>>( d_A, d_B, d_C );
#else
    CUDA_matmul<<< SIZE, THREADS_PER_BLOCK >>>( d_A, d_B, d_C );
#endif

    /* check if the kernel launch was successful */
    err = cudaGetLastError();
    if( err != cudaSuccess ){
        printf( "failed to lauch GPU kernel:\n%s\n", cudaGetErrorString(err) );
        return;
    }

    cudaMemcpy( C, d_C, size_in_bytes, cudaMemcpyDeviceToHost );
                                                                             
    cudaFree( d_A );
    cudaFree( d_B );
    cudaFree( d_C );

    cudaDeviceReset();    
  
}


int main(int argc, char **argv){

  int flag = 0;

  double time, time_cpu, time_gpu;
  double *A, *B, *C, *C_GPU;
  size_t size_in_bytes;
  int i = 0;
  
  size_in_bytes = SIZE* SIZE * sizeof(double);
  
  A = (double *) malloc( size_in_bytes );
  B = (double *) malloc( size_in_bytes );
  C = (double *) malloc( size_in_bytes );
  C_GPU = (double *) malloc( size_in_bytes );
  
  /* fill matrix A & B on cpu */
#pragma omp parallel for private( i )
  for( i = 0; i < SIZE * SIZE; i++ ){
    
    A[i] = rand() / (double) RAND_MAX;
    B[i] = rand() / (double) RAND_MAX;
  }
  
  time = seconds();
  matmul_cpu( A, B , C );
  time_cpu = seconds() - time;
  
  time = seconds();
  matmul_gpu( A, B, C_GPU );
  time_gpu = seconds() - time;
  

  /* check result */
#ifdef __DEBUG

#pragma omp parallel for private( i ) reduction( +:flag )   
  for( i = 0; i < SIZE * SIZE; i++ ) 
    if( fabs( ( C[i] - C_GPU[i] ) / C[i] ) > 0.000001){
      fprintf( stdout, "\nC[%d] = %.3g\t%.3g", i, C[i], C_GPU[i] );
      flag += 1;
    }
#endif
  
  if( !flag ){

    fprintf( stdout, "Program completed successfully!" );
    fprintf( stdout, "Time for CPU code: %g seconds\n", time_cpu );
    fprintf( stdout, "Time for GPU code: %g seconds\n", time_gpu ); 
  }
  else{
    fprintf( stdout, "Program completed unsuccessfully!" );
  }  

  return 0;
}
