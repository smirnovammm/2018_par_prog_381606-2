#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define N 10000

int **m;
int *v;
int * WorckSize;

int timer_s, timer_e;

MPI_Status status;

int main(int argc, char **argv)
{

	srand(time(NULL));

	int numtasks, taskid;

	int perem = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	

	//std::cout << taskid << "\n";

	if (taskid == 0)
	{
		m = new int*[N];
		v = new int[10];

		for (int i = 0; i < 10; i++)
			v[i] = i;

		for (int i = 0; i < N; i++)
			m[i] = new int[N];


		std::cout << "Generating " << N << " Matrix\n";

		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				m[i][j] = rand() % 101 -50;

		std::cout << "Summation...\n";

	//	for (int i = 0; i < N; i++)
//		{
//			for (int j = 0; j < N; j++)
//			{
//							std::cout << m[i][j] << " ";
//			}
//			std::cout << "\n";
//		}



		std::cout << "Runed on " << numtasks << " process\n";


		int ex_out = 0;

		if (numtasks == 1)
		{
			//clock_t start = clock();
			DWORD stime = GetTickCount();
			for (int i = 0; i < N; i++)
				for (int j = 0; j < N; j++)
				{
					ex_out += m[i][j];
				}
			std::cout << "Ansver is " << ex_out << "\n";
			DWORD etime = GetTickCount();
			std::cout << "Time = " << etime - stime << " msc\n";
		//	clock_t end = clock();
			//double seconds = (double)(end - start) / CLOCKS_PER_SEC;
			//std::cout << "Ansver is " << ex_out << "\n";
		//	std::cout.precision(11);
		//	std::cout << "Time = " << seconds << "\n";
			MPI_Finalize();
			return 0;

		}



		DWORD stime = GetTickCount();


		WorckSize = new int[numtasks];
		
		//int ost;
		int ost = N % (numtasks - 1);

		for (int i = 1; i < numtasks; i++)
		{
			WorckSize[i] = N / (numtasks - 1);
			if (ost > 0 && i <= ost) WorckSize[i]++;
		}
		
	//	for (int i = 1; i < numtasks; i++)
	//std::cout << i << " w_s = " << WorckSize[i] << "\n";

	//	std::cout << "StatWork\n";

		for (int i = 1; i < numtasks; i++)
		{
			MPI_Send(WorckSize + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		for (int i = 0; i < N; i++)
		{
	//		std::cout << "n " << i % (numtasks - 1) + 1 << "\n";
			MPI_Send(m[i], N, MPI_INT, i % (numtasks - 1) + 1, 0, MPI_COMM_WORLD);
		}

		int out = 0, b_out = 0;

		for (int i = 0; i < N; i++)
		{
			MPI_Recv(&b_out, 1, MPI_INT, i % (numtasks - 1) + 1, 0, MPI_COMM_WORLD, &status);
			out += b_out;
		}


		DWORD etime = GetTickCount();
		std::cout << "Ansver is " << out << "\n";
		std::cout << "Time = " << etime - stime << " msc\n";

	}
	else
	{
		int myWork;
		int p = 0;
		
		int *w = new int[N];

		MPI_Recv(&myWork, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	//	std::cout << taskid << " w_s_id = " << myWork << "\n";

		for (int i = 0; i < myWork; i++)
		{
			MPI_Recv(w, N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
			int p = 0;
			for (int j = 0; j < N; j++)
			{
			//	std::cout << w[j];
				p += w[j];
			}
			MPI_Send(&p, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}


		
	}





	MPI_Finalize();

	return 0;
}