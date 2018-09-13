#include <iostream>
#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>
#include <math.h>
using namespace std;



void get_walltime_(double* wcTime) {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  *wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);
}

void get_walltime(double* wcTime) {
  get_walltime_(wcTime);
}




int main(){
int n[9] ={100,500,1000,1500,2500,5000,10000,15000,20000};

for(int r=0;r<9;r++){

  int matrix_size=n[r];
  printf("size %d\n",matrix_size);
  /*int a[matrix_size][matrix_size];
  int b[matrix_size][matrix_size];
  int c[matrix_size][matrix_size];*/
  double **a = new double* [matrix_size];
    for(int i = 0; i < matrix_size; i++)
      a[i] = new double[matrix_size];
  double **b = new double* [matrix_size];
    for(int i = 0; i < matrix_size; i++)
      b[i] = new double[matrix_size];
  double **c = new double* [matrix_size];
    for(int i = 0; i < matrix_size; i++)
      c[i] = new double[matrix_size];

  for(int i=0; i<matrix_size;i++){
	  for(int j=0;j<matrix_size;j++){
		  a[i][j]=1;
		  b[i][j]=3;
		  c[i][j]=0;
	}
}



  struct timeval tp, tbefore, tafter;
  gettimeofday(&tbefore, NULL);


  for(int i=0;i<matrix_size;i++){
	   for(int j=0;j<matrix_size;j++){
		     for(int k=0;k<matrix_size;k++){
			        c[i][k] += a[i][j]*b[j][k];

		      }
	    }
  }

  delete [] a;
  delete [] b;
  delete [] c;

  double operations = 2*pow(matrix_size,3.0);
  gettimeofday(&tafter,NULL);
  double time = (double)(tafter.tv_sec+tafter.tv_usec/1000000.0-tbefore.tv_sec-tbefore.tv_usec/1000000.0);
  printf("%lf\n",time);

  double flops = (operations/time)/pow(10,6);
  printf("%lf\n",flops);


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
