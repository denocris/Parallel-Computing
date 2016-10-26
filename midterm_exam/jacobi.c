#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

// function declarations

// print matrix to screen
void print_matrix(double** M, size_t dim);

// save matrix to file
void save_gnuplot(FILE *f, double **M, size_t dim);

// return the elapsed time
double wall_time(void);

int main(int argc, char* argv[]){

  // check on input parameters
  if(argc != 5) {
    fprintf(stderr,"\nwrong number of arguments. Usage: ./a.out dim it n m\n");
    return 1;
  }

  const size_t dimension = atoi(argv[1]);
  const size_t iterations = atoi(argv[2]);
  const size_t row_peek = atoi(argv[3]);
  const size_t col_peek = atoi(argv[4]);

  printf("matrix size = %zu\n", dimension);
  printf("number of iterations = %zu\n", iterations);
  printf("element for checking = Mat[%zu,%zu]\n",row_peek, col_peek);

  if((row_peek > dimension) || (col_peek > dimension)){
    fprintf(stderr, "Cannot Peek a matrix element outside of the matrix dimension\n");
    fprintf(stderr, "Arguments n and m must be smaller than %zu\n", dimension);
    return 1;
  }

  // indexes for loops

  size_t i,j,it;
  
  // initialize matrix
  double **matrix;
  matrix = (double**)malloc(sizeof(double*)*(dimension+2));
  for(i=0; i < dimension+2; ++i)
    matrix[i] = (double*)malloc(sizeof(double)*(dimension+2));
  
  double **matrix_new;
  matrix_new = (double**)malloc(sizeof(double*)*(dimension+2));
  for(i=0; i < dimension+2; ++i){
    matrix_new[i] = (double*)malloc(sizeof(double)*(dimension+2));
    for(j=0; j < dimension+2; ++j)
      matrix_new[i][j] = 0.0;
  }

  //fill initial values  
  for(i=1; i <= dimension; ++i)
    for(j=1; j <= dimension; ++j)
      matrix[i][j] = 0.5;
  
  // set up borders 
  double increment = 100.0/(dimension+1);
  for(i=1; i <= dimension+1; ++i){
    matrix[i][0]=matrix[dimension+1][dimension+1-i]=i*increment;
    matrix_new[i][0]=matrix_new[dimension+1][dimension+1-i]=i*increment;
    matrix[i][dimension+1]=0.0;
    matrix_new[i][dimension+1]=0.0;
    matrix[0][i] = 0.0;
    matrix_new[0][i] = 0.0;
  }
  print_matrix(matrix,dimension); 

  // start algorithm
  double time_start = wall_time();
  for(it = 0; it < iterations; ++it){
    
    //This will be a row dominant program.
    for(i=1;i<=dimension;++i)
      for(j=1;j<=dimension;++j)
	matrix_new[i][j] = (0.25)*(matrix[i-1][j] +
				       matrix[i][j+1] +
				       matrix[i+1][j] +
				       matrix[i][j-1]);
    print_matrix(matrix_new,dimension);
    // swap the pointers
    double ** tmp;
    tmp = matrix;
    matrix = matrix_new;
    matrix_new = tmp;
  }
  double time_end = wall_time();
  printf("\nelapsed time = %f seconds\n", time_end-time_start);
  printf("\nmatrix[%zu,%zu] = %f\n", row_peek, col_peek,
	 matrix[row_peek+1][col_peek+1]);
  FILE *file;
  file = fopen("solution.dat", "w");
  save_gnuplot(file,matrix,dimension);
  fclose(file);

  for(i=0;i<dimension+2;++i){
    free(matrix[i]);
    free(matrix_new[i]);
  }
  free(matrix);
  free(matrix_new);
  return 0;
}

void print_matrix(double**matrix, size_t dim){

  size_t i,j;

  printf("\t");
  for(i=0;i<dim+2;++i){
    printf("%zu\t",i);
  }
  printf("\n");

    
  for(i=0;i<dim+2;++i){
    printf("%zu\t",i);
    for(j=0;j<dim+2;++j){
      printf("%f\t", matrix[i][j]);
    }
    printf("\n");

  }
}

void save_gnuplot(FILE *f, double **M, size_t dim){
  size_t i,j;
  const double h=0.1;
  for (i=0; i<dim+2; ++i)
    for (j=0; j<dim+2; ++j)
      fprintf(f, "%f\t%f\t%f\n", h*i, h*j, M[i][j]);
}

// A Simple timer for measuring the walltime
double wall_time(void){
  static long zsec =0;
  static long zusec = 0;
  double esec;
  struct timeval tp;
  struct timezone tzp;
  gettimeofday(&tp, &tzp);
  if(zsec ==0) zsec  = tp.tv_sec;
  if(zusec==0) zusec = tp.tv_usec;
  esec = (tp.tv_sec-zsec)+(tp.tv_usec-zusec)*0.000001;
  return esec;
}
