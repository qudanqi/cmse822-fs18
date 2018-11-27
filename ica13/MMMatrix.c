#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"
#include "omp.h"

int main(){
// record the wall time
double t1, t2;
// define the matrixes
double **a, **b, **c;
// give the matrix size
int n[3] ={20,100,1000};

for(int r=0;r<3;r++){

  int matrix_size=n[r];
  printf("size %d\n",matrix_size);
// the starting time
  t1 = MPI_Wtime();

  /*
  double **a = new double* [matrix_size];
    for(int i = 0; i < matrix_size; i++)
      a[i] = new double[matrix_size];
  double **b = new double* [matrix_size];
    for(int i = 0; i < matrix_size; i++)
      b[i] = new double[matrix_size];
  double **c = new double* [matrix_size];
    for(int i = 0; i < matrix_size; i++)
      c[i] = new double[matrix_size];
*/
//allocate the memory space for matrixes
a = (double **) malloc(sizeof(double*)*matrix_size);
b = (double **) malloc(sizeof(double*)*matrix_size);
c = (double **) malloc(sizeof(double*)*matrix_size);

for(int i=0; i<matrix_size; i++)
{
  *(a+i) = (double*)malloc(sizeof(double)*matrix_size);
  *(b+i) = (double*)malloc(sizeof(double)*matrix_size);
  *(c+i) = (double*)malloc(sizeof(double)*matrix_size);
}
//initialize the value for matrixes
  for(int i=0; i<matrix_size;i++){
	  for(int j=0;j<matrix_size;j++){
		  a[i][j]=1;
		  b[i][j]=2;
		  c[i][j]=0;
	}
}
double sum;
// doing the multiplication of matrix a and b
// parallelize the two independent nested loop
#pragma omp for collapse(2)
  for(int i=0; i<matrix_size; i++)
	   for(int j=0; j<matrix_size; j++){
       // reset the value sum
       sum = 0;
       // do the reduction of sum
       #pragma omp parallel for reduction(+:sum)
		     for(int k=0; k<matrix_size; k++)
			        sum += a[i][k]*b[k][j];
        c[i][j] = sum;

       }


/*
  delete [] a;
  delete [] b;
  delete [] c;
*/
// free the memory space that allocated for matrixes
free(a);
free(b);
free(c);
//the end time
t2 = MPI_Wtime();
// the total operation time
double time = (t2 - t1);
printf("time = %lf sec\n",time);
// check the result
printf("c[0][0] = %lf\n",c[0][0]);


  //double temp = (double) tafter.tv_sec;
  //double temp2 = (double) tafter.tv_usec/1000000.0;
  //printf("%lf     %lf\n",temp,temp2);

  /*for(int i=0;i<matrix_size;i++){
	 for(int j=0; j<matrix_size; j++){
		printf("%d    ", c[i][j]);
		if(j==matrix_size-1) printf("\n\n");
	   }
   }
   */
}
return 0;
}
