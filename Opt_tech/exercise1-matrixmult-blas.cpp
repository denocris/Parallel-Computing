#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include "mkl.h"

int mnk=4;

double mytime(){
  timeval v;
  gettimeofday(&v,0);
  return v.tv_sec+v.tv_usec/1000000.0;
}

void matrixmul_mnk(double* c,double* a,double* b){
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
	      mnk, mnk, mnk, 1, a, mnk, b, mnk, 1, c, mnk);
}

int main(void){
  int iter=10;
  int nmatrices=100;
  int size=mnk*mnk*nmatrices;
  double* a= (double*) _mm_malloc(sizeof(double)*size,64);
  double* b= (double*) _mm_malloc(sizeof(double)*size,64);
  double* c= (double*) _mm_malloc(sizeof(double)*size,64);
  double time1,time2;
  double time1_N,time2_N;
  for(int i=0;i<size;i++){
    a[i]=rand();
    b[i]=rand();
    c[i]=rand();
  }

  time1=mytime();
  for(int n=0;n<iter;n++){
    for(int i=0;i<size;i+=mnk*mnk){
      matrixmul_mnk(&c[i],&a[i],&b[i]);
	  //you code goes here
      //matrixmul_mnk_opt1(&c[i],&a[i],&b[i]);
    }
  }
  time2=mytime();

  for(int i=0;i<size;i++){
    c[i]=rand();
  }

  // time1_N=mytime();
  // for(int n=0;n<iter;n++){
  //   for(int i=0;i<size; i++)
  //     for(int j=0;j<size; j++)
  //       for(int k=0;k<size; k++){
  //     c[i][j] +=  a[i][k]*b[k][j];
  //   }
  // }
  // time2_N=mytime();

  time1_AN=mytime();
  for(int n=0;n<iter;n++){
    for(int i=0;i<size; i++){
      c[i][:]+=a[i][0:size]*b[0:size][i];
    }
  }
  time2_AN=mytime();



  printf("time cblas = %f s\n", time2-time1);
  printf("time normal = %f s\n", time2_N-time1_N);
  printf("perf = %f GFLOPs\n", (2.0*mnk*mnk*mnk*nmatrices*iter)/(time2-time1)/1000.0/1000.0/1000.0);
}
