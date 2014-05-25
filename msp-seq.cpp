/*
 * A naive sequential algorithm for the MPI assignment
 * (for corretness testing on small data).
 * Concurrent and Distributed Programming Course, spring 2014.
 * Faculty of Mathematics, Informatics and Mechanics.
 * University of Warsaw, Warsaw, Poland.
 *
 * Copyright (C) Piotr Sokolski, 2014.
 */
#include "common.h"
#include <iostream>

using namespace std;

void safe_exit(int code){
  exit(code);
}

int main(int argc, char * argv[])
{
  initialize(argc, argv);
  
  startTiming();
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
    safe_exit(0);
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
