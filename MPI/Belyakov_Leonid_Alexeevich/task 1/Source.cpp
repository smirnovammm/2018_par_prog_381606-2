#include "mpi.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

int Initilization(double* &matrix, int n,  int min_range, int max_range) 
{
	srand(time(NULL));
	for (int i = 0; i < n*n; i++)
	{
		matrix[i] = rand() % max_range + min_range;
	}
	return 0;
}


double* FindMin(double* &matrix, int n, int DataSize)
{
	double* min = new double[n];
	for (int i = 0; i < DataSize; i++)
		min[i] = 65000;

	for (int i = 0; i < (DataSize); i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (matrix[j + i*n] < min[i])
			{
				min[i] = matrix[j + i*n];
			}
		}
	}
	return min;
}

bool AreEqual(double* &array1, double* &array2, int Rows)
{
	for (int i = 0; i < Rows; i++)
	{
		if (array1[i]!=array2[i])
			return false;
	}
	return true;
}
void main(int argc, char** argv)
{
	int N = atoi(argv[1]);
	int rank, RankSize;
	double TimeStart, TimeEnd;
	double* matrix = new double[N*N];	
	int DataSize;	
	double *RecvBuf =NULL;
	double* minSeq=new double[N];
	double* minMPI = new double[N];
	Initilization(matrix, N, 0, 10);
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &RankSize);
	//Последовательная версия
	if (rank == 0) 
	{
		cout << "Sequential" << endl;
		TimeStart = MPI_Wtime();
		minSeq = FindMin(matrix, N, N);
		TimeEnd = MPI_Wtime();
		cout << "Time: " << TimeEnd - TimeStart << endl;
	}
	//Параллельная версия
	if (rank == 0)
	{
		cout << "MPI" << endl;
	}
	TimeStart = MPI_Wtime();
	DataSize = N/RankSize;
	MPI_Bcast(&DataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	RecvBuf = new double[DataSize*N];
	if (rank == 0)
	{
		for (int i = 1; i < RankSize; i++)
		{
			MPI_Send(&matrix[i*(N*DataSize)], N*DataSize, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
		}
		RecvBuf = matrix;
	}
	else
	{
		MPI_Recv(RecvBuf, N*(DataSize), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	double* minMPI_datasize = FindMin(RecvBuf, N, DataSize); 
	MPI_Gather(minMPI_datasize, DataSize, MPI_DOUBLE, minMPI, N / RankSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	TimeEnd = MPI_Wtime();
	if (rank == 0)
	{
		cout << "Time: " << TimeEnd - TimeStart << endl;
		if (!AreEqual(minSeq, minMPI, N))
		{
			cout << "Error: minSeq != minMPI" << endl;
		}
		else
		{
			cout << "Success: minSeq == minMPI" << endl;
		}
		delete[] matrix;
		delete[] minMPI;
		delete[] minSeq;
	}
	MPI_Finalize();
}