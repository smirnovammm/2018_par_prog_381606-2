#include <mpi.h>
#include <iostream>
#include <vector>
#include<time.h>
using namespace std;

void main(int argv, char* argc[])
{
	int size_v, procNum, procRank, localSize;
	double start, finish;
	int sum = 0, res = 0;
	size_v = 100000;
	int *v, *localv ;


	v = new int[size_v];
	srand(time(NULL));
	for (int i = 0; i < size_v; i++)
		v[i] = rand() % 100;

	MPI_Init(&argv, &argc);
	MPI_Comm_size(MPI_COMM_WORLD, &procNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

	if (procRank == 0)
		start = MPI_Wtime();

	MPI_Bcast(&size_v, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (procNum == 0)
	{
		int addSize;
		if (!(size_v % procNum))
		{
			int tmp, *arr, newSize;
			tmp = size_v / procNum + 1;
			addSize = tmp*procNum - size_v;
			newSize = size_v + addSize;
			arr = new int[size_v];
			for (int i = 0; i < size_v; i++)
				arr[i] = v[i];
			delete[] v;
			v = new int[newSize];
			for (int i = 0; i < size_v; i++)
				v[i] = arr[i];
			for (int i = size_v; i < newSize; i++)
				v[i] = 0;
			size_v = newSize;
			delete[] arr;
		}
	}

	localv = new int[size_v / procNum];
	MPI_Scatter(v, size_v / procNum, MPI_INT, localv, size_v / procNum, MPI_INT, 0, MPI_COMM_WORLD);
	
	for (int i = 0; i < size_v/procNum; i++)
		sum += localv[i];

	MPI_Reduce(&sum, &res, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (procRank == 0)
	{
		finish = MPI_Wtime();
		cout << "result = " << res << endl;
		cout << "time = " << finish - start<<endl;
	}

	MPI_Finalize();
}