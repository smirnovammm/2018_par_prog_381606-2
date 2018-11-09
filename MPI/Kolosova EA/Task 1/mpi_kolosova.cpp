// mpi_kolosova.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "mpi.h" 
#include <stdio.h>
#include <iostream> 
#include <fstream>
#include "function.h"

using namespace std;

int main(int argc, char *argv[])
{
	int* ROW_SUM = NULL;
	int sum = 0;
	int* matrix = NULL;
	int procNum, procRank;
	int row = 10, column = 10;
	//int* row_summs = NULL;
	int* packSumms = NULL;
	int pack = 0;


	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &procNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

	double startTime = 0;
	double finishTime = 0;

	//if (procNum <= 1) 
		//return 1;

	if (procRank == 0)
	{
		ROW_SUM = create_new_vector(row);//результат
		matrix = create_new_vector(row * column);//исходная матриц

		fill_vector(matrix, row, column, 9);
		fill_summs_null(ROW_SUM, row);

		output_vector(matrix, row, column);

		startTime = MPI_Wtime();
	}

	pack  = row / procNum;
	if (procRank == 0)
		cout << "pack " << pack << endl;
	
	int* got_data = new int[pack * column];
	packSumms = create_new_vector(row);
	//cout << "will scatter" << endl;
	MPI_Scatter(matrix, pack * column, MPI_INT, got_data, pack * column, MPI_INT, 0, MPI_COMM_WORLD);
	//cout << "did scatter" << endl;
	
	//cout << "will sum" << procRank << endl;
	for (int i = 0; i < pack; i++)
	{
		sum = 0;
		for (int j = 0; j < column; j++)
		{
			sum += got_data[i * column + j];
		}
		packSumms[i] = sum;
		//cout << "packSumms " << packSumms[i] << " " << i << endl;
	}
	//cout << "did sum" << procRank << endl;
	//MPI_Barrier(MPI_COMM_WORLD);

	//cout << "will gatter" << procRank << endl;
	MPI_Gather(packSumms, pack, MPI_INT, ROW_SUM, pack, MPI_INT, 0, MPI_COMM_WORLD);
	//cout << "did gatter" << procRank << endl;

	int tail = row % procNum;
	if(procRank == 0)
		cout << "tail " << tail << endl;

	if (tail != 0)
	{
		//cout << "will count tail" << endl;

		int resCount = pack*column*procNum;//число уже отработанных элементов
		int sumCount = pack * procNum;

		if (procRank == 0)
		{
			for (int i = 0; i < tail; i++)
			{
				sum = 0;
				for (int j = 0; j < column; j++)
				{
					sum += matrix[(resCount + i*column) + j];
				}
				ROW_SUM[sumCount + i] = sum;
			}
		}
	}

	if (procRank == 0)
	{
		finishTime = MPI_Wtime();
		output_summs(ROW_SUM, row);
		cout << "Time: " << startTime - finishTime;
	}

	MPI_Finalize();

	delete[] matrix;
	delete[] ROW_SUM;
	delete[] got_data;
	delete[] packSumms;
	return 0;
}