#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <random>

std::default_random_engine generator(time(0));
std::uniform_int_distribution <int> dist(0, 100);

void CreateMatrix(int* matrix, int columns, int lines)
{
	for (int i = 0; i < columns * lines; i++)
	{
		matrix[i] = dist(generator);
	}
}

void PrintMatrix(int* matrix, int columns, int lines)
{
	for (int i = 0; i < lines; i++)
	{
		for(int j = 0; j < columns; j++)
		{
			std::cout << matrix[i + j * lines] << "\t";
		}
		std::cout << std::endl;
	}

}

void FindMax(int* matr, int* arr_max, int columns, int lines)
{
	int temp_max = 0;
	for (int i = 0; i < columns; i++)	//on columns
	{
		for (int j = 0; j < lines; j++)	//on lines
		{
			if (matr[i * lines + j] > temp_max)
			{
				temp_max = matr[i * lines + j];
			}
		}
		arr_max[i] = temp_max;
		temp_max = 0;
	}
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);    
	
	MPI_Status Status;
	
	int lines = atoi(argv[1]);
	int columns = atoi(argv[2]);	

	int ProcRank, ProcNum;
	
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);                                  
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);                           
	
	int block = columns / ProcNum;                                
	int* matrix = new int[columns*lines];        
	int* matrix_tmp = new int[block*lines];  
	int* max_par = new int[columns];                                                         
	int* max_tmp_vec = new int[block]; 

	if (ProcRank == 0) 
	{
		CreateMatrix(matrix, columns, lines);
		PrintMatrix(matrix, columns, lines);

		//Parallel algorithm:

		for (int i = 0; i < ProcNum - 1; i++)
		{
			matrix_tmp = matrix + i * block * lines;
			MPI_Send(matrix_tmp, block*lines, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
		}
		for (int j = 1; j < ProcNum; j++)
		{
			MPI_Recv(max_tmp_vec, block, MPI_INT, j, 0, MPI_COMM_WORLD, &Status);
			for (int i = 0; i < block; i++)
			{
				max_par[((j - 1) * block + i)] = max_tmp_vec[i];
			}
		}
		int max = 0;
		for (int i = ((ProcNum-1) * block); i < columns; i++)	//the last process takes on a remain
		{
			for (int j = 0; j < lines ; j++)
			{
				if (matrix[i * lines + j] > max)
					max = matrix[i * lines + j];
			}
			max_par[i] = max;
			max = 0;
		}
		std::cout << "Maximum element (parallel algorithm): " << std::endl;
		for (int i = 0; i < columns; i++)
		{
			std::cout<< max_par[i] << "\t";
		}
		std::cout << std::endl;
	}
	else
	{
		MPI_Recv(matrix_tmp, block * lines, MPI_INT, 0, 0, MPI_COMM_WORLD, &Status);
		FindMax(matrix_tmp, max_tmp_vec, block, lines);
		MPI_Send(max_tmp_vec, block, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}
