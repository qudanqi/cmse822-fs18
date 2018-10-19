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
    // define the window name
    MPI_Win window1, window2;
    // scan the total number
    int nGlobal = atoi(argv[1]);

    // Setup local array
    int nLocal = (int)nGlobal/numRanks;
    if (myRank==0) nLocal += nGlobal%numRanks;

    // Allocate and initialize the vector
    // x is allocate with 2 ghost zones
    std::vector<float> x(nLocal+2);
    std::vector<float> y(nLocal);
    // give value to x array for every process
    for (int i=1; i<nLocal+1; i++){
        x[i] = i + myRank*(nLocal+nGlobal%numRanks);
    }

    // Now specify neighbors
    int leftNeigh = myRank-1;
    if (myRank==0) leftNeigh = numRanks-1;
    int rghtNeigh = myRank+1;
    if (myRank==numRanks-1) rghtNeigh = 0;
    // create windows for data communication
    MPI_Win_create(&x[0],2*sizeof(float),sizeof(float),MPI_INFO_NULL,MPI_COMM_WORLD,&window1);
    MPI_Win_create(&x[nLocal],2*sizeof(float),sizeof(float),MPI_INFO_NULL,MPI_COMM_WORLD,&window2);


    // Put the most left value to the leftNeighbour and get the most right value from it
    MPI_Win_fence(0,window2);

    MPI_Put(&x[1],1,MPI_FLOAT,leftNeigh,1,1,MPI_FLOAT,window2);
    MPI_Get(&x[0],1,MPI_FLOAT,leftNeigh,0,1,MPI_FLOAT,window2);
    // Calculate the average exlude the most left and right
    for (int i = 0; i<nLocal; i++)
    {
        y[i] = (x[i]+x[i+1]+x[i+2])/3;
    }

    MPI_Win_fence(0,window2);

    // Get the most right value from the leftNeighbour
    /*MPI_Win_fence(0,window2);
    MPI_Get(&x[0],1,MPI_FLOAT,leftNeigh,0,1,MPI_FLOAT,window2);
    MPI_Win_fence(0,window2); */

    // free windows
    MPI_Win_free(&window1);
    MPI_Win_free(&window2);

    // perform the averaging of the boundary
    y[0] = (x[0]+x[1]+x[2])/3;
    y[nLocal-1] = (x[nLocal-1]+x[nLocal]+x[nLocal+1])/3;
    /*
    if (myRank == 0){
      for(int i = 0; i<=nLocal+1; i++){
        cout << "x[" << i <<"]=" << x[i] << endl;
      }
    }
    */
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
