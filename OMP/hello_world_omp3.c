#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main( int argc, char * argv[]){

int i = 0, j = 0;
int idx = 0;


#pragma omp parallel private(idx)
{

	idx = omp_get_thread_num();
	printf("After parall region I am %d \n", idx);


// Try all of them and look the differences
//#pragma omp
//#pragma omp for collapse(2)
//#pragma omp for private(i,j)

#pragma omp for private(i,j) collapse(2) // set always to private indices of loops

for( i = 0; i < 4; i++){
	for(j = 0; j < 4; j++){
printf("Myid = %d\t A[%d][%d] \n", idx, i, j);

}
}




}

return 0;
}
