/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + nrows*(__j)])

void *process(void *args);
//class argument;

typedef struct argument {
  char* outboard;
  char* inboard;
  int nrows;
  int ncols;
  int row_start;
  int num_rows;
}argument;



char *game_of_life (char* outboard, char* inboard, const int nrows, const int ncols, const int gens_max) { 

  int num_threads = 4;

  int i,curgen;
  int start_row = 0;
  int num_rows_per_thread = nrows / num_threads;

  /* Initialize arguments for threads */
  argument *args = malloc(num_threads * sizeof(argument));
  for (i=0; i<num_threads; i++) {
    args[i].row_start = start_row;
    args[i].num_rows = num_rows_per_thread;
    args[i].nrows = nrows;
    args[i].ncols = ncols;

    start_row += num_rows_per_thread;
  }

  /* Create threads */
  pthread_t threads[num_threads];

  for (curgen=0; curgen<gens_max; curgen++) {

    memmove (outboard, inboard, nrows * ncols * sizeof (char));

    for (i=0; i<num_threads; i++) {
      /* Update inboard and outboard after each generation */
      args[i].outboard = outboard;
      args[i].inboard = inboard;

      pthread_create(&threads[i], NULL, process, (void*)&args[i]);
    }

    for (i=0; i<num_threads; i++){
      pthread_join(threads[i], NULL);
    }

    SWAP_BOARDS( outboard, inboard );
  }

  free(args);
  return inboard;
}


void *process(void *args)
{
  int i, j;
  argument *val = (argument *) args;

  char* outboard = val->outboard;
  char* inboard = val->inboard;

  int row_start = val->row_start;
  int row_end = val->row_start + val->num_rows;

  int nrows = val->nrows;
  int ncols = val->ncols;

  /* HINT: you'll be parallelizing these loop(s) by doing a
  geometric decomposition of the output */
  for (i = row_start; i < row_end; i++){
    for (j = 0; j < ncols; j++){
      const int inorth = mod (i-1, nrows);
      const int isouth = mod (i+1, nrows);
      const int jwest = mod (j-1, ncols);
      const int jeast = mod (j+1, ncols);

      const char neighbor_count = 
        BOARD (inboard, inorth, jwest) + 
        BOARD (inboard, inorth, j) + 
        BOARD (inboard, inorth, jeast) + 
        BOARD (inboard, i, jwest) +
        BOARD (inboard, i, jeast) + 
        BOARD (inboard, isouth, jwest) +
        BOARD (inboard, isouth, j) + 
        BOARD (inboard, isouth, jeast);

      BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));
    }
  }

  pthread_exit(NULL);
}



