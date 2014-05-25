#include <mpi.h>
#include "common.h"
#include <cmath>

void safe_exit(int code){
  MPI_Finalize();
  exit(code);
}

int nth_length(int n, int sum){
  return std::ceil((std::sqrt(8.0*(sum - n) + 1.0) - 1.0)/2.0);
}

int main(int argc, char * argv[])
{
  MPI_Init(&argc, &argv);
  int numProcesses, myRank;
  MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  
  initialize(argc, argv);
  
  startTiming();
  
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
    if (myRank == 0)
      printEnd();
    free(matrixPtr);
    safe_exit(0);
  }
  
  int total_subsets = (numRows + 1)*numRows/2;
  
  for (int i = myRank; i < total_subsets; i += numProcesses){
    int sub_length = nth_length(i, total_subsets);
    int k = numRows - sub_length + 1;
    int sub_start = total_subsets - sub_length*(sub_length+1)/2;
    int pos_in_sub = i - sub_start;
    int l = k + pos_in_sub;
    
    std::cout << myRank << ": " << k << ' ' << l << std::endl;
    
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

  if (myRank == 0)
    printEnd();
  free(matrixPtr);
  safe_exit(0);
}
