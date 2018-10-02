#include <iostream>
#include "mpi.h"
using namespace std;

int main() 
{
    cout << "1: Hello, World!" << endl;
	MPI_Init(NULL,NULL);
	cout<<"2: Hello, World!" << endl;
	char name[MPI_MAX_PROCESSOR_NAME];
	int result[80];
	MPI_Get_processor_name(name,result);
	cout << name<<endl;
	MPI_Finalize();
		cout<< "3: Hello, World!" << endl;
    return 0;
}