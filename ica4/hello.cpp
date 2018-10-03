#include <iostream>
#include "mpi.h"
using namespace std;

int main()
{
    cout << "Hello, Before!" << endl;

    MPI_Init(NULL,NULL);
    cout << "Hello, World!" << endl;
    MPI_Finalize();

    cout << "Hello, After!" << endl;
    return 0;
}
