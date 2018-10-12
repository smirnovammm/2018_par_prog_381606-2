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
	int rank, size, i , tmp;
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
	v1 = new double[VecSize];
	v2 = new double[VecSize];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	displs = new int[size];
	sendcounts = new int[size];

	if (rank == 0) {
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

		Step = VecSize / size;
		tmp = VecSize % size;
		sendcounts = new int[size];
		displs = new int[size];
		i = rank;
		for (int i = 0; i < size; i++) {
			if (tmp != 0) {
				sendcounts[i] = Step + 1;
				tmp--;
			}
			else {
				sendcounts[i] = Step;
			}
		}
		displs[0] = 0;
		for (int i = 1; i < size; i++) {
			displs[i] = displs[i - 1] + sendcounts[i - 1];
		}
		//MPI_Bcast(&tmp, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}
	MPI_Bcast(&tmp, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&Step, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(sendcounts, size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(displs, size, MPI_INT, 0, MPI_COMM_WORLD);

	for (int i = 0; i < size; i++) {
		if (rank < tmp) {
			v11 = new double[Step + 1];
			v12 = new double[Step + 1];
		}
		else {
			v11 = new double[Step];
			v12 = new double[Step];
		}
	}

	//MPI_Bcast(v1, VecSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	//MPI_Bcast(v2, VecSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatterv(v1, sendcounts,  displs, MPI_DOUBLE, v11, sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatterv(v2, sendcounts, displs, MPI_DOUBLE, v12, sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

	for (int i = 0; i < sendcounts[rank]; i++) {
		ProcSum += v1[i] * v2[i];
	}

	/*for (int j = Step * i; j < Step * i + Step; j++)
		ProcSum += v1[j] * v2[j];

	if (size * Step + rank < VecSize) {
		i = size * Step + rank;
		ProcSum += v1[i] * v2[i];
	}*/

	MPI_Reduce(&ProcSum, &TotalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0) { 
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