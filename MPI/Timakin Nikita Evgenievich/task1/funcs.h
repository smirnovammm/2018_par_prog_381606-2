#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define VECTOR_SIZE 20

void randomize_vector(int *vec, int len); // заполнение массива случайными числами
int maximum(int *vec, int len); // нахождение максимального элемента в массиве
int *complete_vector(int *vec, int *len, int divider, int rem); // добавление фиктивных компонент к ветору
void print_vector(int *vec, int len); // печать вектора