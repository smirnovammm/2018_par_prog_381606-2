#include<mpi.h>
#include<iostream>
#include<time.h>

//char* genstr() {
//	int size = rand() % 300;
//	char* str = new char[size];
//	int i;
//	for (i = 0; i < size; i++) {
//		str[i] = rand() % 256;
//	}
//	str[i + 1] = '\0';
//	return str;
//}

int main()
{
	//srand(time(NULL));
	int sizeofproc, numofproc;
	char* str = { "Статья - это текст, посвященный одной конкретной теме, опубликованный в журнале, газете или каком-либо издании" }; // 110 -> 96
	//char *str = genstr();
	
	int length = strlen(str);
	MPI_Init(nullptr, nullptr);
	MPI_Comm_size(MPI_COMM_WORLD, &sizeofproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &numofproc);
	MPI_Status status;

	if (sizeofproc == 1 && numofproc == 0) {
		double time = MPI_Wtime();
		int count = 0;
		for (int i = 0; i < length; i++) {
			if ((str[i] != ' ') && (str[i] != '\t') && (str[i] != 'n') && (str[i] != '\0') && (str[i] != '\0'))
			{
				count++;
			}
		}

		time = MPI_Wtime() - time;
		std::cout << "LENGTH: " << length << std::endl;
		std::cout << "TIME OF THE 0 PROCESS: " << time << std::endl;
		std::cout << "RESULT: " << count << std::endl;

		MPI_Finalize();
		return 0;
	}

	if (numofproc == 0) {
		double time = MPI_Wtime();
		int *tempcount = new int();
		int count = 0;
		for (int i = 1; i < sizeofproc; i++) {
			MPI_Recv(tempcount, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			count += *tempcount;
		}
		time = MPI_Wtime() - time;
		std::cout << "LENGTH: " << length << std::endl;
		std::cout << "TIME OF THE 0 PROCESS: " << time << std::endl;
		std::cout << "RESULT: " << count << std::endl;
	}
	else {
		if (length % (sizeofproc - 1) != 0) {
			double time = MPI_Wtime();
			int count = 0;
			int templength = length / (sizeofproc - 1);
			int tailtemplength = length % (sizeofproc - 1);
			if (numofproc != sizeofproc - 1) {
				for (int i = templength*(numofproc - 1); i < templength*(numofproc - 1) + templength; i++) {

					if ((str[i] != ' ') && (str[i] != '\t') && (str[i] != 'n') && (str[i] != '\0') && (str[i] != '\0'))
					{
						count++;
					}
				}
			}
			else {
				for (int i = templength*(numofproc - 1); i < templength*(numofproc - 1) + templength + tailtemplength; i++) {

					if ((str[i] != ' ') && (str[i] != '\t') && (str[i] != 'n') && (str[i] != '\0') && (str[i] != '\0'))
					{
						count++;
					}
				}
			}
			MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			time = MPI_Wtime() - time;
			std::cout << "TIME OF THE " << numofproc << " PROCESS: " << time << std::endl;
		}

		else {
			double time = MPI_Wtime();
			int count = 0;
			int templength = length / (sizeofproc - 1);

			for (int i = templength*(numofproc - 1); i < templength*(numofproc - 1) + templength; i++) {

				if ((str[i] != ' ') && (str[i] != '\t') && (str[i] != 'n') && (str[i] != '\0') && (str[i] != '\0'))
				{
					count++;
				}

			}

			MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			time = MPI_Wtime() - time;
			std::cout << "TIME OF THE " << numofproc << " PROCESS: " << time << std::endl;
		}
	}

	MPI_Finalize();

	return 0;
}