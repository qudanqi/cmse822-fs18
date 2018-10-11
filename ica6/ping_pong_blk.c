# include <stdio.h>
# include <math.h>
# include <stdlib.h>
# include "mpi.h"

int main (int argc, char *argv[])
{
double t1, t2, tt;
int i, j, n, m;
int num, numsize;
n = pow(2,22);
/*int (*message)[n];
message = (int (*)[]) malloc(n*sizeof(int));*/
static int message[1024*1024*4];
// Initialize the MPI environment
MPI_Init(NULL, NULL);
MPI_Comm comm;
comm = MPI_COMM_WORLD;
// Give the rank and size
int world_rank;
MPI_Comm_rank(comm, &world_rank);
int world_size;
MPI_Comm_size(comm, &world_size);
//Calculate the walltime
for (m = 0; m <= 22; m++){
  num = 1;
  numsize = 1;
for (j = 1; j <= m; j++){
  num = 2*num;
}
 numsize = num*sizeof(int);
t1 = MPI_Wtime();
//rank 0 and rank size-1 do the ping-pong

for (i=1; i<=1000;i++){
if (world_rank == 0){
  MPI_Send(&message, num, MPI_INT, world_size-1, 0, comm);
  MPI_Recv(&message, num, MPI_INT, world_size-1, 0, comm, MPI_STATUS_IGNORE);

}
else if (world_rank == world_size-1){
  MPI_Recv(&message, num, MPI_INT, 0, 0, comm, MPI_STATUS_IGNORE);
  MPI_Send(&message, num, MPI_INT, 0, 0, comm);
}
/*if (world_rank == 0){
  printf("%d/n",i);
}*/
}
//Calculate the walltime
t2 = MPI_Wtime();
tt = (t2 - t1)/1000;
if (world_rank == 0){
printf("the total communication time of message %d is %lg second\n", numsize, tt);
}
}
//MPI_Finalize
MPI_Finalize();
//free(message);
return 0;
}
