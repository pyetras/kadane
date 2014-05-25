//
//  common.h
//  zad3
//
//  Created by Piotr Sokólski on 25.05.2014.
//  Copyright (c) 2014 Piotr Sokólski. All rights reserved.
//

#ifndef __zad3__common__
#define __zad3__common__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

typedef long long int matrix_t;

extern int numRows, numColumns, seed;

extern int bestI, bestJ, bestK, bestL;
extern struct timeval startTime;
extern matrix_t max_sum;
extern matrix_t *matrixPtr;

void printMatrix(matrix_t const * m, int r, int c);
void printEnd();
void printUsage(char const * prog);

void initialize(int argc, char * argv[]);
void startTiming();

static inline int at(int x, int y){
  return x * (numColumns + 1) + y;
}

#endif /* defined(__zad3__common__) */
