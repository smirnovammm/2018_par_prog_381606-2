#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

void randomize_vector(int *vec, int len);
int maximum(int *vec, int len);
int *complete_vector(int *vec, int *len, int divider, int rem);
void print_vector(int *vec, int len);
