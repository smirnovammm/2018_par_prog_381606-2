#include <mpi.h>
#include "funcs.h"

int main(int argc, char *argv[])
{
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
	{
		printf("MPI initialization is failed\n");
		return 1;
	}
	else
	{
		int proc_count, rank, local_size, local_max, global_max, global_size = 0;
		int *vec = NULL, *local_vec = NULL;
		int i;
		const int root = 0;
		int rem;

		MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		rem = global_size % proc_count;

		if (rank == root)
		{
			if (argc > 1)
			{
				global_size = atoi(argv[1]);
			}
			else
			{
				global_size = 20;
			}

			vec = (int *)malloc(global_size * sizeof(int));
			//srand(time(NULL));
			randomize_vector(vec, global_size);

			print_vector(vec, global_size);
			printf("\n");
		}
		
		MPI_Bcast(&global_size, 1, MPI_INT, root, MPI_COMM_WORLD);

		local_size = global_size / proc_count;
		local_vec = (int *)malloc(local_size * sizeof(int));

		MPI_Scatter(vec, local_size, MPI_INT, local_vec, local_size, MPI_INT, root, MPI_COMM_WORLD);

		local_max = maximum(local_vec, local_size);
		MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, root, MPI_COMM_WORLD);

		if (rank == root)
		{
			printf("global_max = %d", global_max);
		}

		MPI_Finalize();
	}

	return 0;
}
