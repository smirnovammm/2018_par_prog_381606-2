#include <iostream>
#include <ctime>
#include "MPI.h"


int main(int argc, char* argv[])
{
	long long int* matrix = 0;
	int width, height;
	int max = 0, GlobalMax = 0;
	srand(6);

	int ProcSize, procID;
	int *send_counts, *displacements, *RecieveBuffer;
	int localBuff, reminder;

	double time;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &procID);

	width = atoi(argv[1]);
	height = atoi(argv[2]);

	if (procID == 0)
	{
		matrix = new long long int[width * height];
		for (int i = 0; i < width * height; ++i)
			matrix[i] = rand();

		if ((width < 10) && (height < 10))
			for (int i = 0; i < width; ++i)
			{
				for (int j = 0; j < height; ++j)
					std::cout << matrix[i * height + j] << "  ";
				std::cout << "\n";
			}
	}

	time = MPI_Wtime();
	send_counts = new int[ProcSize];
	displacements = new int[ProcSize];
	localBuff = width * height / ProcSize;
	reminder = (width * height) % ProcSize; /// elements remaining after division among processes

	send_counts[0] = localBuff + reminder;
	displacements[0] = 0;
	for (int i = 1; i < ProcSize; ++i)
	{
		send_counts[i] = localBuff;
		displacements[i] = reminder + i * localBuff;
	}

	RecieveBuffer = new int[send_counts[procID]];

	///Divide the data among processes 
	MPI_Scatterv(matrix, send_counts, displacements, MPI_INT, RecieveBuffer, send_counts[procID], MPI_INT, 0, MPI_COMM_WORLD);

	max = RecieveBuffer[0];
	for (int i = 0; i < send_counts[procID]; ++i) ///Maximum Search
		if (max < RecieveBuffer[i])
			max = RecieveBuffer[i];

	MPI_Reduce(&max, &GlobalMax, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);  ///Transfer of maximum from local buffers to ROOT proc

	if (procID == 0)
	{
		time = MPI_Wtime() - time;

		std::cout << "Max is: " << GlobalMax << std::endl;
		std::cout << "Time spent: " << time << std::endl;

		delete matrix;
	}

	MPI_Finalize();

	delete[] send_counts;
	delete[] displacements;
	delete[] RecieveBuffer;

	return 0;
}
