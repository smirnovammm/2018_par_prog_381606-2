#include "funcs.h"

void randomize_vector(int *vec, int len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		vec[i] = rand() % 100;
	}
}

int maximum(int *vec, int len)
{
	int i, max = vec[0];

	for (i = 1; i < len; i++)
	{
		if (vec[i] > max)
			max = vec[i];
	}

	return max;
}

int *complete_vector(int *vec, int *len, int divider, int rem)
{
	int i, new_len = *len + divider - rem;
	int *res = (int *)malloc(new_len * sizeof(int));

	for (i = 0; i < *len; i++)
	{
		res[i] = vec[i];
	}
	for (i = *len; i < new_len; i++)
	{
		res[i] = INT_MIN;
	}
	*len = new_len;
	free(vec);

	return res;
}

void print_vector(int *vec, int len)
{
	int i;
	printf("[ ");
	for (i = 0; i < len; i++)
	{
		printf("%d ", vec[i]);
	}
	printf("]");
}