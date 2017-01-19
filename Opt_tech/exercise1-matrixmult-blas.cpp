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
  for(int i=0;i<size;i++){
    a[i]=rand();
    b[i]=rand();
    c[i]=rand();
  }

  //-------------- NAIVE IMPLEMENTATION ----------

  time1=mytime();
  for(int m=0;m<iter;m++)
    for(int n=0;n<nmatrices;n++){
      matrixmul_naive(c, a, b);
  }
  time2=mytime();

  printf("matrix C1 = %f \n", c[3]);


  //-------------- CBLAS IMPLEMENTATION ----------

  for(int i=0;i<size;i++){
    c[i]=rand();
  }

  time1_cblas=mytime();
  for(int n=0;n<iter;n++){
    for(int i=0;i<size;i+=mnk*mnk){
      matrixmul_mnk(&c[i],&a[i],&b[i]);
    }
  }
  time2_cblas=mytime();

  printf("matrix C1 = %f \n", c[3]);

  //-------------- ARRAY NOTATION IMPL ----------

  for(int i=0;i<size;i++){
    c[i]=rand();
  }

  time1_AN=mytime();
  for(int m=0;m<iter;m++)
    for(int n=0;n<nmatrices;n++){
      matrixmul_mnk_AN(c, a, b);
  }
  time2_AN=mytime();

  printf("matrix C3 = %f \n", c[3]);

  //------------------------------------------

  printf("time naive = %f s\n", time2-time1);
  printf("time cblas = %f s\n", time2_cblas-time1_cblas);
  printf("time AN = %f s\n", time2_AN-time1_AN);
  printf("perf naive = %f GFLOPs\n", (2.0*mnk*mnk*mnk*nmatrices*iter)/(time2-time1)/1000.0/1000.0/1000.0);
  printf("perf cblas = %f GFLOPs\n", (2.0*mnk*mnk*mnk*nmatrices*iter)/(time2_cblas-time1_cblas)/1000.0/1000.0/1000.0);
  printf("perf AN = %f GFLOPs\n", (2.0*mnk*mnk*mnk*nmatrices*iter)/(time2_AN-time1_AN)/1000.0/1000.0/1000.0);
}
