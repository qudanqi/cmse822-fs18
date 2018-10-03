#include <stdio.h>
#include "mpi.h"
int main()
{
   printf("Hello, before!\n");

   MPI_Init(NULL,NULL);
   printf("Hello, World!\n");
   MPI_Finalize();

   printf("Hello, after!\n");
   return 0;
}
