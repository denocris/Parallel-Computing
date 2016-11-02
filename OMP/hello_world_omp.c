#include <stdlib.h>
#include <omp.h>
#include <stdio.h>

int main( int argc, char * argv[]){


#pragma omp parallel
{
	printf("I am %d of %d threads",
      omp_get_thread_num(), omp_get_num_threads());
}

return 0;
}
