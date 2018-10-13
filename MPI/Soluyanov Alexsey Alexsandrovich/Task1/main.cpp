#include<mpi.h>
#include<iostream>
#include <ctime>
using namespace std;
double Sequental_Executing(double *v1, double *v2, int size) {
	double Total_Res = 0;
	for (int i = 0; i < size; i++) {
		Total_Res += v1[i] * v2[i];
	}
	return Total_Res;
}

int Scalar_Product(int argc, char **argv)
{
	double t1, t2, dt, t3, t4, dt2, Saq_Res;
	int rank, size, i , tmp, newtmp;
	double TotalSum = 0, ProcSum = 0;
	int VecSize;
	double *v1 = NULL;
	double *v2 = NULL;
	double *v11 = NULL;
	double *v12 = NULL;
	int Step;
	int *sendcounts, *displs;

	if (argc > 1) {
		VecSize = atoi(argv[1]);
	}
	else {
		VecSize = 10;
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	displs = new int[size];
	sendcounts = new int[size];
	sendcounts = new int[size];
	displs = new int[size];

	Step = VecSize / size;
	tmp = VecSize % size;
	i = rank;
	for (int i = 0; i < tmp; i++) {
		sendcounts[i] = Step + 1;
	}
	for (int i = tmp; i < size; i++) {
		sendcounts[i] = Step;
	}
	displs[0] = 0;
	for (int i = 1; i < size; i++) {
		displs[i] = displs[i - 1] + sendcounts[i - 1];
	}

	if (rank == 0) {
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


		t3 = MPI_Wtime();
		Saq_Res = Sequental_Executing(v1, v2, VecSize);
		t4 = MPI_Wtime();
		dt2 = t4 - t3;
		cout << "The execution time of the sequental program is " << dt2 << " sec" << endl;

		t1 = MPI_Wtime();
	}


	if (rank < tmp) {
		v11 = new double[Step + 1];
		v12 = new double[Step + 1];
	}
	else {
		v11 = new double[Step];
		v12 = new double[Step];
	}

	//MPI_Bcast(v1, VecSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	//MPI_Bcast(v2, VecSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatterv(v1, sendcounts,  displs, MPI_DOUBLE, v11, sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatterv(v2, sendcounts, displs, MPI_DOUBLE, v12, sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

	for (int i = 0; i < sendcounts[rank]; i++) {
		ProcSum += v11[i] * v12[i];
	}

	/*for (int j = Step * i; j < Step * i + Step; j++)
		ProcSum += v1[j] * v2[j];

	if (size * Step + rank < VecSize) {
		i = size * Step + rank;
		ProcSum += v1[i] * v2[i];
	}*/
	
	MPI_Reduce(&ProcSum, &TotalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) { 
		cout << endl << "TOTAL SUM (Parallel Programm) = " << TotalSum << endl;
		if (TotalSum == Saq_Res) {
			cout << "The results of the sequential and parallel algorithms are the same. The decision is correct." << endl;
		}
		t2 = MPI_Wtime();
		dt = t2 - t1;
		cout << "The execution time of the parallel program is " << dt << " sec" << endl; 
	}
	MPI_Finalize();
	delete[] v1;
	delete[] v2;
	return 0;
}


int main(int argc, char **argv)
{
	Scalar_Product(argc, argv);
}