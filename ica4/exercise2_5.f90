program exercise2_5
    implicit none
    include "mpif.h"

    INTEGER :: comm
    INTEGER :: size, rank
    INTEGER :: ierr


    MPI_Init(ierr)

    MPI_Comm_size(comm,size,ierr)

    if (size == 0)
    then MPI_Comm_rank(comm,rank,ierr)
    print*, size
    end if

    MPI_Finalize(ierr)

end program exercise2_5
