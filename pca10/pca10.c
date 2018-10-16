#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
int main(int argc,char **argv) {

MPI_Init(&argc, &argv);
MPI_Comm comm;
comm = MPI_COMM_WORLD;
MPI_Win the_window;
srand((unsigned int) time(NULL));

int nprocs, procno;

MPI_Comm_size(comm, &nprocs);
MPI_Comm_rank(comm, &procno);
int my_number, other_number[2], other;
other = (int) rand()%2 + 1;

if (procno==0)
my_number = 27;
//snippet getfence
MPI_Win_create(&other_number,2*sizeof(int),sizeof(int),MPI_INFO_NULL,comm,&the_window);
MPI_Win_fence(0,the_window);
if (procno==0) {
MPI_Put( &my_number, 1,MPI_INT,other, 0, 1,MPI_INT,the_window);
}
MPI_Win_fence(0,the_window);

MPI_Bcast(&other, 1, MPI_INT, 0, comm);
//snippet end
if (procno==0)
printf("I write the following: %d to rank %d\n",my_number, other);
MPI_Win_free(&the_window);

MPI_Finalize();
return 0;
}
