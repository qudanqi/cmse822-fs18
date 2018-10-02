#include <iostream>
#include "mpi.h"
#include <fstream>
using namespace std;

int main()
{

    MPI_Init(NULL, NULL);

    int total_processes;
    int current_process;

    ofstream out;
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &current_process);
    out.open("file_" + to_string(current_process) + ".txt");
    out << "This is process " << current_process << " of " << total_processes <<endl;
    out.close();

    MPI_Finalize();
    return 0;
}
