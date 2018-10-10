#include<mpi.h>
#include<iostream>
#include <ctime>
using namespace std;
int Sequental_Executing(double *v1, double *v2, int size) {
	clock_t start_time;
	clock_t end_time;
	clock_t total_time;
	int i = 0;
	start_time = clock();
	double Total_Res = 0;
	for (int i = 0; i < size; i++) {
		Total_Res += v1[i] * v2[i];
	}
	end_time = clock();
	total_time = end_time - start_time;
	cout << "The result of the scalar product = " << Total_Res << endl;
	cout << "Sequental time is " << double(total_time) / double(CLOCKS_PER_SEC) << " sec"  << endl;
	return 0;
}

int Scalar_Product(int argc, char **argv)
{
	double t1, t2, dt;
	int rank, size, i;
	double TotalSum = 0, ProcSum = 0;
	int VecSize;
	double *v1 = NULL;
	double *v2 = NULL;
	int Step;

	if (argc > 1) {
		VecSize = atoi(argv[1]);
	}
	else {
		VecSize = 10;
	}
	v1 = new double[VecSize];
	v2 = new double[VecSize];
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		srand(time(NULL));
		v1 = new double[2 * VecSize];
		v2 = v1 + VecSize;
		v1 = new double[VecSize];
		v2 = new double[VecSize];
		for (int i = 0; i < VecSize; i++) {
			v1[i] = rand() % 10;
			v2[i] = rand() % 10;
		}
		if (VecSize < 15) {
			cout << "VECTORS : " << endl << " ( ";

			for (int i = 0; i < VecSize; i++) {
				cout << v1[i] << " ";
			}
			cout << ") " << endl << " ( ";

			for (int i = 0; i < VecSize; i++) {
				cout << v2[i] << " ";
			}
			cout << ") " << endl;
		}

		Sequental_Executing(v1, v2, VecSize);

		t1 = MPI_Wtime();
	}
	Step = VecSize / size;
	i = rank;

	MPI_Bcast(v1, VecSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(v2, VecSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);


	for (int j = Step * i; j < Step * i + Step; j++)
		ProcSum += v1[j] * v2[j];

	if (size * Step + rank < VecSize) {
		i = size * Step + rank;
		ProcSum += v1[i] * v2[i];
	}

	MPI_Reduce(&ProcSum, &TotalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0) { 
		t2 = MPI_Wtime();
		dt = t2 - t1;
		cout << "The execution time of the parallel program is " << dt << " sec" << endl; 
	}
	//delete[] v1;
	//delete[] v2;
	MPI_Finalize();
	delete[] v1;
	delete[] v2;
	return 0;
}


int main(int argc, char **argv)
{
	Scalar_Product(argc, argv);
}