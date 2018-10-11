#include <iostream>
#include <fstream>
#include <cstdlib>
#include <mpi.h>
#include <vector>
#include <string>


using namespace std;

int main(int argc, char *argv[])
{
    // Initialize the MPI communicator
    MPI_Init(&argc, &argv);

    // Get rank/size
    int myRank, numRanks;

    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numRanks);

    MPI_Request *requests = (MPI_Request*) malloc(4*sizeof(MPI_Request));
    int nGlobal = 100;

    // Setup local array
    int nLocal = (int)nGlobal/numRanks;
    if (myRank==0) nLocal += nGlobal%numRanks;

    // Allocate and initialize the vector
    // x is allocate with 2 ghost zones
    std::vector<float> x(nLocal+2);
    std::vector<float> y(nLocal);
    for (int i=1; i<nLocal+1; i++){
        x[i] = i + myRank*(nLocal+nGlobal%numRanks);
    }

    // Now specify neighbors
    int leftNeigh = myRank-1;
    if (myRank==0) leftNeigh = numRanks-1;
    int rghtNeigh = myRank+1;
    if (myRank==numRanks-1) rghtNeigh = 0;

// Use non-blocking communication to send messages
    MPI_Isend(&x[1], 1, MPI_FLOAT, leftNeigh, 0, MPI_COMM_WORLD, requests);

    MPI_Isend(&x[nLocal], 1, MPI_FLOAT, rghtNeigh, 0, MPI_COMM_WORLD,requests+1 );

    MPI_Irecv(&x[0], 1, MPI_FLOAT, leftNeigh, 0, MPI_COMM_WORLD, requests+2);

    MPI_Irecv(&x[nLocal+1], 1, MPI_FLOAT, rghtNeigh, 0, MPI_COMM_WORLD, requests+3);

// perform the averaging exclude the boundary while the communication is processing
    for (int i = 1; i<nLocal-1; i++)
   {
        y[i] = (x[i]+x[i+1]+x[i+2])/3.0;
    }
//Wait until the communication has been done
   MPI_Waitall(4, requests, MPI_STATUS_IGNORE);
// perform the averaging of the boundary
   y[0] = (x[0]+x[1]+x[2])/3.0;
   y[nLocal-1] = (x[nLocal-1]+x[nLocal]+x[nLocal+1])/3.0;
// open files to print out the results
  ofstream fout;
  fout.open("Average"+to_string(myRank)+".txt");
   for(int i = 0; i<=nLocal-1;i++){
     fout <<"Rank = " <<myRank<<", Local ID = " << i << ", average = " << y[i] <<"\r"<< endl;
}
// close files
  fout.close();
// MPI finalize
  MPI_Finalize();
}
