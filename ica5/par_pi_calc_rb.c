/***************************************************************************
 * FILE: ser_pi_calc.c
 * DESCRIPTION:
 *   Serial pi Calculation - C Version
 *   This program calculates pi using a "dartboard" algorithm.  See
 *   Fox et al.(1988) Solving Problems on Concurrent Processors, vol.1
 *   page 207.
 * AUTHOR: unknown
 * REVISED: 02/23/12 Blaise Barney
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


void srandom (unsigned seed);
double dboard (int darts);

#define DARTS 1000   	/* number of throws at dartboard */
#define ROUNDS 128    	/* number of times "darts" is iterated */

int main(int argc, char *argv[])
{
double pi;          	/* average of pi after "darts" is thrown */
double avepi = 0;       	/* average pi value for all iterations */
double total_pi =0;  // sum reduced pi using mpi
double wtime = 0, twtime = 0;
int i, n;
int world_size;       //number of processors
int world_rank;
printf("Starting parallel version of pi calculation using dartboard algorithm...\n");

MPI_Init(NULL,NULL);
MPI_Comm_size(MPI_COMM_WORLD, &world_size);
MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
srandom(5*world_rank+1);        //seed with different random number for each rank
double t1 = MPI_Wtime();
for (i = 0; i < (ROUNDS/world_size); i++)
 {
   /* Perform pi calculation on serial processor */
   pi = dboard(DARTS);
   MPI_Reduce(&pi,&total_pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
   if(world_rank == 0)
    {
      avepi = ((avepi*i*world_size) + total_pi)/(world_size*(i + 1));
      printf("   After %3d throws, average value of pi = %10.8f\n",
         (DARTS * world_size * (i + 1)),avepi);
    }
  }
double t2 = MPI_Wtime();
wtime = t2 - t1;
MPI_Reduce(&wtime,&twtime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
/*printf("time: %10.8f/n", t2-t1);*/
if(world_rank == 0){
printf("time: %10.8f\n", twtime/(double)world_size);
printf("Real value of PI: 3.1415926535897 \n");
}
MPI_Finalize();
}


/*****************************************************************************
 * dboard
 *****************************************************************************/
#define sqr(x)	((x)*(x))
long random(void);

double dboard(int darts)
{
   double x_coord,       /* x coordinate, between -1 and 1  */
          y_coord,       /* y coordinate, between -1 and 1  */
          pi,            /* pi  */
          r;             /* random number scaled between 0 and 1  */
   int score,            /* number of darts that hit circle */
       n;
   unsigned int cconst;  /* must be 4-bytes in size */
/*************************************************************************
 * The cconst variable must be 4 bytes. We check this and bail if it is
 * not the right size
 ************************************************************************/
   if (sizeof(cconst) != 4) {
      printf("Wrong data size for cconst variable in dboard routine!\n");
      printf("See comments in source file. Quitting.\n");
      exit(1);
      }
   /* 2 bit shifted to MAX_RAND later used to scale random number between 0 and 1 */
   cconst = 2 << (31 - 1);
   score = 0;

/***********************************************************************
 * Throw darts at board.  Done by generating random numbers
 * between 0 and 1 and converting them to values for x and y
 * coordinates and then testing to see if they "land" in
 * the circle."  If so, score is incremented.  After throwing the
 * specified number of darts, pi is calculated.  The computed value
 * of pi is returned as the value of this function, dboard.
 ************************************************************************/
   int total_score =0;

   for (n = 1; n <= darts; n++) {
      /* generate random numbers for x and y coordinates */
      r = (double)random()/cconst;
      x_coord = (2.0 * r) - 1.0;
      r = (double)random()/cconst;
      y_coord = (2.0 * r) - 1.0;

      /* if dart lands in circle, increment score */
      if ((sqr(x_coord) + sqr(y_coord)) <= 1.0)
         score++;
      }


   /* calculate pi */
   pi = 4.0 * (double)score/(double)darts;
   return(pi);
}
