//
//  common.cpp
//  zad3
//
//  Created by Piotr Sokólski on 25.05.2014.
//  Copyright (c) 2014 Piotr Sokólski. All rights reserved.
//

#include "common.h"
#include "matgen.h"

int numRows, numColumns, seed;

int bestI, bestJ, bestK, bestL;
struct timeval startTime;
matrix_t max_sum;
matrix_t * matrixPtr;

extern void safe_exit(int code);

void printMatrix(matrix_t const * m, int r, int c)
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

void printEnd(){
  struct timeval     endTime;
  if (gettimeofday(&endTime, NULL))
  {
    fprintf(stderr, "ERROR: Gettimeofday failed!\n");
    safe_exit(1);
  }
  
  double duration =
  ((double) endTime.tv_sec + ((double) endTime.tv_usec / 1000000.0)) -
  ((double) startTime.tv_sec + ((double) startTime.tv_usec / 1000000.0));
  
  fprintf(stderr, "PWIR2014_Piotr_Sokolski_292408 Input: (%d,%d,%d) Solution: |(%d,%d),(%d,%d)|=%lld Time: %.10f\n",
          numRows, numColumns, seed,
          bestJ, bestI, bestK, bestL, max_sum, duration);
}

void printUsage(char const * prog)
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "    %s <num_rows> <num_colums> <seed>\n\n", prog);
}


void initialize(int argc, char * argv[]){
  matgen_t * matgenPtr;
  
  if (argc != 4)
  {
    fprintf(stderr, "ERROR: Invalid arguments!\n");
    printUsage(argv[0]);
    safe_exit(1);
  }
  numRows = atoi(argv[1]);
  numColumns = atoi(argv[2]);
  seed = atoi(argv[3]);
  if (numRows <= 0 || numColumns <= 0 || seed <= 0)
  {
    fprintf(stderr, "ERROR: Invalid arguments: %s %s %s!\n", argv[1],
            argv[2], argv[3]);
    printUsage(argv[0]);
    safe_exit(1);
  }
  matgenPtr = matgenNew(numRows, numColumns, seed);
  if (matgenPtr == NULL)
  {
    fprintf(stderr, "ERROR: Unable to create the matrix generator!\n");
    printUsage(argv[0]);
    safe_exit(1);
  }
  matrixPtr = (matrix_t *) malloc(sizeof(matrix_t) * (numRows + 1) * (numColumns + 1));
  if (matrixPtr == NULL)
  {
    fprintf(stderr, "ERROR: Unable to create the matrix!\n");
    printUsage(argv[0]);
    safe_exit(1);
  }
  for (int j = 1; j <= numRows; ++j)
  {
    for (int i = 1; i <= numColumns; ++i)
    {
      matrixPtr[j * (numColumns + 1) + i] = matgenGenerate(matgenPtr);
    }
  }
  matgenDestroy(matgenPtr);
}

void startTiming(){
  if (gettimeofday(&startTime, NULL))
  {
    fprintf(stderr, "ERROR: Gettimeofday failed!\n");
    safe_exit(1);
  }
}

