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

		MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);

		if (rank == root)
		{
			int rem;

			global_size = VECTOR_SIZE;

			vec = (int *)malloc(global_size * sizeof(int));
			//srand(time(NULL));
			randomize_vector(vec, global_size);

			rem = global_size % proc_count;
			if (rem != 0)
			{
				vec = complete_vector(vec, &global_size, proc_count, rem);
			}

			print_vector(vec, global_size);
			printf("\n");
		}
		
		MPI_Bcast(&global_size, 1, MPI_INT, root, MPI_COMM_WORLD);

		local_size = global_size / proc_count;
		local_vec = (int *)malloc(local_size * sizeof(int));

		if (rank == root)
		{
			local_vec = vec;
			for (i = 1; i < proc_count; i++)
			{
				MPI_Send(&vec[local_size * i], local_size, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
		}
		else
		{
			MPI_Recv(local_vec , local_size, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

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
