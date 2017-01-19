#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <immintrin.h> // for the intrinsic implementation

#include "mkl.h"

int mnk=4;

double mytime(){
  timeval v;
  gettimeofday(&v,0);
  return v.tv_sec+v.tv_usec/1000000.0;
}

void matrixmul_naive(double* c,double* a,double* b){
    for(int i=0; i<4; i++)
      for(int j=0; j<4; j++)
        for(int k=0; k<4; k++){
          c[i*4 + j]+=a[i*4 + k]*b[k*4 + j];
      }
}

void matrixmul_mnk(double* c,double* a,double* b){
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
	      mnk, mnk, mnk, 1, a, mnk, b, mnk, 1, c, mnk);
}


void matrixmul_mnk_AN(double* c,double* a,double* b){
    for(int i=0; i<4; i++)
      for(int k=0; k<4; k++){
        c[i*4:4]+=a[i*4 + k]*b[k*4:4];
      }
}

// void matrixmul_intrinsic(double* c, double* a, double* b){
//   __m256d a_line, b_line, c_line;
//     for(int i=0;i<mnk*mnk;i+=4){
//     // unroll loop to avoid initializing c_line to zero
//         a_line = _mm256_load_pd(a); //a_line =vec4(column(a,0))
//         b_line = _mm256_set1_pd(b[i]); //b_line = vec4(b[i][0]), Broadcast double-precision (64-bit) floating-point value b[i]
//         c_line = _mm256_mul_pd(a_line,b_line);
//         for (int j = 1; j < 4; j++) {
//             a_line = _mm256_load_pd(&a[j*4]); //a_line = vec4(column(a,j))
//             b_line = _mm256_set1_pd(b[i+j]); //b_line = vec4(b[i][j])
//             c_line = _mm256_add_pd(_mm256_mul_pd(a_line,b_line), c_line); //r_line += a_line*b_line
//         }
//         _mm256_store_pd(&c[i], c_line);// r[i] = r_line
//     }
// }

void matrixmul_intrinsic(double* c, double* a, double* b){
  __m256d a_line, b_line, c_line;
      for(int i=0;i<mnk*mnk;i+=4){
        for (int j = 0; j < 4; j++) {
            a_line = _mm256_load_pd(&a[j*4]); //a_line = vec4(column(a,j))
            b_line = _mm256_set1_pd(b[i+j]); //b_line = vec4(b[i][j])
            c_line = _mm256_add_pd(_mm256_mul_pd(a_line,b_line), c_line); //r_line += a_line*b_line
        }
        _mm256_store_pd(&c[i], c_line);// r[i] = r_line
      }

}

int main(void){
  int iter=10;
  int nmatrices=100000;
  int size=mnk*mnk*nmatrices;
  double* a= (double*) _mm_malloc(sizeof(double)*size,64);
  double* b= (double*) _mm_malloc(sizeof(double)*size,64);
  double* c= (double*) _mm_malloc(sizeof(double)*size,64);
  double time1,time2;
  double time1_cblas,time2_cblas;
  double time1_AN,time2_AN;
  double time1_intrinsic,time2_intrinsic;
  for(int i=0;i<size;i++){
    a[i]=2.0f;
    b[i]=3.0f;
    c[i]=0.0f;
  }

  printf("matrix C0 = %f \n", c[3]);

  //-------------- NAIVE IMPLEMENTATION ----------

  time1=mytime();
  for(int n=0;n<iter;n++)
    for(int i=0;i<size;i+=mnk*mnk){
      matrixmul_naive(&c[i], &a[i], &b[i]);
  }

  time2=mytime();

  printf("matrix C1 = %f \n", c[0]);


  //-------------- CBLAS IMPLEMENTATION ----------

  for(int i=0;i<size;i++){
    c[i]=0.0f;
  }

  time1_cblas=mytime();
  for(int n=0;n<iter;n++)
    for(int i=0;i<size;i+=mnk*mnk){
      matrixmul_mnk(&c[i],&a[i],&b[i]);
    }

  time2_cblas=mytime();

  printf("matrix C2 = %f \n", c[0]);

  //-------------- ARRAY NOTATION IMPL ----------

  for(int i=0;i<size;i++){
    c[i]=0.0f;
  }

  time1_AN=mytime();
  for(int n=0;n<iter;n++)
    for(int i=0;i<size;i+=mnk*mnk){
      matrixmul_mnk_AN(&c[i],&a[i],&b[i]);
  }

  time2_AN=mytime();

  printf("matrix C3 = %f \n", c[0]);

  //-------------- INTRINSIC IMPLEMENTATION ----------

  for(int i=0;i<size;i++){
    c[i]=0.0f;
  }

  time1_intrinsic=mytime();
  for(int n=0;n<iter;n++)
    for(int i=0;i<size;i+=mnk*mnk){
      matrixmul_intrinsic(&c[i],&a[i],&b[i]);
  }

  time2_intrinsic=mytime();

  printf("matrix C4 = %f \n", c[0]);

  //--------------------------------------------------

  printf("time naive = %f s\n", time2-time1);
  printf("time cblas = %f s\n", time2_cblas-time1_cblas);
  printf("time AN = %f s\n", time2_AN-time1_AN);
  printf("time Intrs = %f s\n", time2_intrinsic-time1_intrinsic);
  printf("perf naive = %f GFLOPs\n", (2.0*mnk*mnk*mnk*nmatrices*iter)/(time2-time1)/1000.0/1000.0/1000.0);
  printf("perf cblas = %f GFLOPs\n", (2.0*mnk*mnk*mnk*nmatrices*iter)/(time2_cblas-time1_cblas)/1000.0/1000.0/1000.0);
  printf("perf AN = %f GFLOPs\n", (2.0*mnk*mnk*mnk*nmatrices*iter)/(time2_AN-time1_AN)/1000.0/1000.0/1000.0);
  printf("perf Intrs = %f GFLOPs\n", (2.0*mnk*mnk*mnk*nmatrices*iter)/(time2_intrinsic-time1_intrinsic)/1000.0/1000.0/1000.0);
}
