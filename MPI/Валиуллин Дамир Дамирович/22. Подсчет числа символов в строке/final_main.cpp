#include<mpi.h>
#include<iostream>
#include<time.h>

int main()
{

	int sizeofproc, numofproc;
	int sizeofglobstr = 0, sizeoflocstr = 0, tail = 0;
	int* argvlochalsize = nullptr;
	int* argvtail = nullptr;
	char* str = nullptr;
	char* recvbuff = nullptr;

	MPI_Init(nullptr, nullptr);
	MPI_Comm_size(MPI_COMM_WORLD, &sizeofproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &numofproc);
	double wtime;
	

	if (numofproc == 0) {
		srand(time(nullptr));
		int size =rand() % 100000 + 50000;
		str = new char[size];
		for (int i = 0; i < size - 1; i++) {
			int k = rand() % 10;
			if (k == 0) {
				str[i] = ' ';
			}
		else {
			str[i] = rand() % 100 + 50;
		}
		}
		str[size-1] = '\0';
		wtime = MPI_Wtime();

		//str = new char[120];
		//str = { "dsfafauij fgerfgrgjewoigjtkgj rgoigjotrigjiodjgioriogfghd shdthtgrd grsgrghdh dgzfzfesfr gro;ikstgsgrgdvrffsjkvrfaffrfrr" }; //length = 120,  globcount = 114
		argvlochalsize = new int[sizeofproc];
		argvtail = new int[sizeofproc];
		

		sizeofglobstr = strlen(str);
		tail = sizeofglobstr % sizeofproc;
		sizeoflocstr = sizeofglobstr / sizeofproc;
		
		for (int i = 0; i < sizeofproc; i++) {
			argvlochalsize[i] = sizeoflocstr;
			argvtail[i] = 0;
		}
		argvtail[sizeofproc - 1] = tail;

	}

	int *displs = new int[sizeofproc];
	for (int i = 0; i < sizeofproc; i++) {
		displs[i] = i;
	}
	int *sendcounts = new int[sizeofproc];
	for (int i = 0; i < sizeofproc; i++) {
		sendcounts[i] = 1;
	}

	MPI_Scatterv(argvlochalsize, sendcounts, displs, MPI_INT, &sizeoflocstr, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatterv(argvtail, sendcounts, displs, MPI_INT, &tail, 1, MPI_INT, 0, MPI_COMM_WORLD);


	std::cout <<numofproc<<": "<< sizeoflocstr<<" " <<tail <<std::endl;

		recvbuff = new char[sizeoflocstr+tail];

		MPI_Scatter(str + sizeoflocstr*numofproc, sizeoflocstr+tail, MPI_CHAR, recvbuff, sizeoflocstr + tail, MPI_CHAR, 0, MPI_COMM_WORLD);
		int loccount = 0, globcount = 0;

		for (int i = 0; i < sizeoflocstr+tail; i++) {
			if ((recvbuff[i] != ' ') && (recvbuff[i] != '\t') && (recvbuff[i] != '\n') && (recvbuff[i] != '\0')) {
				loccount++;
			}
		}
		MPI_Reduce(&loccount, &globcount,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

		if (numofproc == 0) {
			wtime = MPI_Wtime() - wtime;
			std::cout << "TIME: " << wtime << "; COUNT: " << globcount << std::endl;
		}
	
	MPI_Finalize();
	return 0;
}