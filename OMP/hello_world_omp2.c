#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main( int argc, char * argv[]){

int a=2, b=3;

printf("Before parallerl region a=%d\t b=%d \n",a,b);

#pragma omp parallel private(a) firstprivate(b)
{
	b = (omp_get_thread_num() % 2) *b;

	printf("I am %d of %d threads. a=%d\t b=%d \n", omp_get_thread_num(), omp_get_num_threads(),a,b);
}
printf("After parallerl region a=%d\t b=%d \n",a,b);
return 0;
}
