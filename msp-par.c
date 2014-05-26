#include <mpi.h>
#include "common.h"
#include <cmath>
#include <iostream>

using namespace std;

void safe_exit(int code){
  MPI_Finalize();
  exit(code);
}

int nth_length(int n, int sum){
  return std::ceil((std::sqrt(8.0*(sum - n) + 1.0) - 1.0)/2.0);
}

typedef struct s {
  matrix_t local_max_sum;
  int bestI, bestJ, bestK, bestL;
}result_t;

void my_max(void *_in, void *_inout, int *len, MPI_Datatype *type){
  result_t *in = (result_t*)_in;
  result_t *inout = (result_t*)_inout;
  if (in->local_max_sum > inout->local_max_sum)
    *inout = *in;
}

int main(int argc, char * argv[])
{
  MPI_Init(&argc, &argv);
  int numProcesses, myRank;
  MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  
  initialize(argc, argv);
  
#ifdef PROFILING
  cout << myRank << ":params:" << argv[1] << ' ' << argv[2] << ' ' << argv[3] << ' ' << numProcesses << endl;
  double baseTime = MPI_Wtime();
#endif
  
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
  
#ifdef PROFILING
  cout << myRank << ":prefixSum:" << MPI_Wtime() - baseTime << endl;
#endif
  
  int total_subsets = (numRows + 1)*numRows/2;
  
  result_t local_result;
  local_result.local_max_sum = max_sum;

  for (int i = myRank; i < total_subsets; i += numProcesses){
    int sub_length = nth_length(i, total_subsets);
    int k = numRows - sub_length + 1;
    int sub_start = total_subsets - sub_length*(sub_length+1)/2;
    int pos_in_sub = i - sub_start;
    int l = k + pos_in_sub;
    
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
    
    if (row_max_sum > local_result.local_max_sum){
      local_result.local_max_sum = row_max_sum;
      local_result.bestJ = k;
      local_result.bestI = row_start;
      local_result.bestK = l;
      local_result.bestL = row_finish;
    }
    
  }
  
#ifdef PROFILING
  cout << myRank << ":work:" << MPI_Wtime() - baseTime << endl;
#endif
  
  MPI_Op myop;
  MPI_Datatype mytype;
  
  MPI_Aint extent;
  MPI_Type_extent(MPI_LONG_LONG_INT, &extent);
  
  MPI_Aint offsets[] = {0, extent};
  MPI_Datatype oldtypes[] = {MPI_LONG_LONG_INT, MPI_INT};
  int blockcounts[] = {1, 4};
  
  MPI_Type_struct(2, blockcounts, offsets, oldtypes, &mytype);
  MPI_Type_commit(&mytype);
  
  MPI_Op_create(my_max, true, &myop);
  
  result_t result;
  
#ifdef PROFILING
  cout << myRank << ":startReduce:" << MPI_Wtime() - baseTime << endl;
#endif

  MPI_Reduce(&local_result, &result, 1, mytype, myop, 0, MPI_COMM_WORLD);
  
#ifdef PROFILING
  cout << myRank << ":endReduce:" << MPI_Wtime() - baseTime << endl;
#endif
  
  if (myRank == 0){
    max_sum = result.local_max_sum;
    bestI = result.bestI;
    bestJ = result.bestJ;
    bestK = result.bestK;
    bestL = result.bestL;
  
    printEnd();
  }
  free(matrixPtr);
  MPI_Finalize();
  return 0;
}
