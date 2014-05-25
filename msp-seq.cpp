/*
 * A naive sequential algorithm for the MPI assignment
 * (for corretness testing on small data).
 * Concurrent and Distributed Programming Course, spring 2014.
 * Faculty of Mathematics, Informatics and Mechanics.
 * University of Warsaw, Warsaw, Poland.
 *
 * Copyright (C) Piotr Sokolski, 2014.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "matgen.h"

typedef long long int matrix_t;

static void printUsage(char const * prog)
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "    %s <num_rows> <num_colums> <seed>\n\n", prog);
}


static int numRows, numColumns, seed;

static int bestI, bestJ, bestK, bestL;
static struct timeval startTime;
static matrix_t max_sum;

static inline int at(int x, int y){
  return x * (numColumns + 1) + y;
}

static void printMatrix(long long int const * m, int r, int c)
{
//  int i, j;
//  for (i = 0; i <= r; ++i)
//  {
//    for (j = 0; j <= c; ++j)
//    {
//      printf(" %lld", m[at(i, j)]);
//    }
//    printf("\n");
//  }
//  printf("\n");
}

static void printEnd(){
  struct timeval     endTime;
  if (gettimeofday(&endTime, NULL))
  {
    fprintf(stderr, "ERROR: Gettimeofday failed!\n");
    exit(1);
  }

  double duration =
  ((double) endTime.tv_sec + ((double) endTime.tv_usec / 1000000.0)) -
  ((double) startTime.tv_sec + ((double) startTime.tv_usec / 1000000.0));
  
  fprintf(stderr, "PWIR2014_Piotr_Sokolski_292408 Input: (%d,%d,%d) Solution: |(%d,%d),(%d,%d)|=%lld Time: %.10f\n",
          numRows, numColumns, seed,
          bestJ, bestI, bestK, bestL, max_sum, duration);
}

int main(int argc, char * argv[])
{
  matgen_t * matgenPtr;
  matrix_t * matrixPtr;
  
  if (argc != 4)
  {
    fprintf(stderr, "ERROR: Invalid arguments!\n");
    printUsage(argv[0]);
    exit(1);
  }
  numRows = atoi(argv[1]);
  numColumns = atoi(argv[2]);
  seed = atoi(argv[3]);
  if (numRows <= 0 || numColumns <= 0 || seed <= 0)
  {
    fprintf(stderr, "ERROR: Invalid arguments: %s %s %s!\n", argv[1],
            argv[2], argv[3]);
    printUsage(argv[0]);
    exit(1);
  }
  matgenPtr = matgenNew(numRows, numColumns, seed);
  if (matgenPtr == NULL)
  {
    fprintf(stderr, "ERROR: Unable to create the matrix generator!\n");
    printUsage(argv[0]);
    exit(1);
  }
  matrixPtr = (matrix_t *) malloc(sizeof(matrix_t) * (numRows + 1) * (numColumns + 1));
  if (matrixPtr == NULL)
  {
    fprintf(stderr, "ERROR: Unable to create the matrix!\n");
    printUsage(argv[0]);
    exit(1);
  }
  for (int j = 1; j <= numRows; ++j)
  {
    for (int i = 1; i <= numColumns; ++i)
    {
      matrixPtr[j * (numColumns + 1) + i] = matgenGenerate(matgenPtr);
    }
  }
  matgenDestroy(matgenPtr);
  
  if (gettimeofday(&startTime, NULL))
  {
    fprintf(stderr, "ERROR: Gettimeofday failed!\n");
    exit(1);
  }
  
  // *** START OF COMPUTATION ***
  
  for (int i = 0; i <= numColumns; ++i)
  {
    matrixPtr[at(0, i)] = 0;
  }
  for (int j = 1; j <= numRows; ++j)
  {
    matrixPtr[at(j, 0)] = 0;
  }
  printMatrix(matrixPtr, numRows, numColumns);
  max_sum = matrixPtr[numColumns + 1 + 1];
  bestI = bestJ = bestK = bestL = 1;
  for (int j = 1; j <= numRows; ++j)
  {
    for (int i = 1; i <= numColumns; ++i)
    {
      if (matrixPtr[at(j, i)] > max_sum){
        max_sum = matrixPtr[at(j, i)];
        bestJ = j;
        bestI = i;
        bestK = j;
        bestL = i;
      }
      matrixPtr[at(j, i)] +=
      matrixPtr[at(j-1, i)];
    }
  }
  
  if (max_sum < 0){
    printEnd();
    free(matrixPtr);
    return 0;
  }
  
  printMatrix(matrixPtr, numRows, numColumns);
  
  for (int k = 1; k <= numRows; ++k)
  {
    for (int l = k; l <= numRows; ++l)
    {
      matrix_t sum = 0, row_max_sum = 0;
      int row_start = 1, row_finish = 0;
      int current_start = 1;
      for (int j = 1; j <= numColumns; ++j){
        matrix_t val = matrixPtr[at(l, j)] - matrixPtr[at(k-1, j)];
        sum += val;
        if (sum < 0){
          current_start = j + 1;
          sum = 0;
        }else if (sum > row_max_sum){
          row_start = current_start;
          row_max_sum = sum;
          row_finish = j;
        }
      }
      
      if (row_max_sum > max_sum){
        max_sum = row_max_sum;
        bestJ = k;
        bestI = row_start;
        bestK = l;
        bestL = row_finish;
      }
    }
  }
  
  // *** END OF COMPUTATION ***
  
  printEnd();
  free(matrixPtr);
  return 0;
}
