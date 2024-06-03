// optimized versions of matrix diagonal summing
#include "sumdiag.h"

////////////////////////////////////////////////////////////////////////////////
// REQUIRED: Paste a copy of your sumdiag_benchmark from an ODD grace
// node below.
//
// -------REPLACE WITH YOUR RUN + TABLE --------
// 
/* 

grace9:~/216/projects/p5-code: ./sumd
sumdiag_benchmark* sumdiag_print*
grace9:~/216/projects/p5-code: ./sumdiag_benchmark
==== Matrix Diagonal Sum Benchmark Version 5 ====
------ Tuned for ODD grace.umd.edu machines -----
Running with 5 sizes (max 8192) and 4 thread_counts (max 4)
  SIZE   BASE #T   OPTM  SPDUP POINTS
  1024  0.029  1  0.015   2.02   0.23
               2  0.008   3.72   0.43
               3  0.010   2.83   0.34
               4  0.009   3.42   0.40
  1101  0.031  1  0.016   1.98   0.24
               2  0.008   3.73   0.46
               3  0.010   3.16   0.41
               4  0.012   2.67   0.35
  2048  0.209  1  0.052   4.04   0.92
               2  0.026   7.91   1.36
               3  0.024   8.68   1.42
               4  0.027   7.60   1.33
  4099  1.320  1  0.228   5.78   2.31
               2  0.117  11.28   3.18
               3  0.110  12.04   3.27
               4  0.117  11.28   3.18
  8192  7.779  1  0.897   8.68   5.67
               2  0.446  17.45   7.51
               3  0.319  24.39   8.39
               4  0.281  27.67   8.72
RAW POINTS: 50.13
 _   _                 _   _           _   _       _____ _    ____ _____ _
| \ | | _____      __ | |_| |__   __ _| |_( )___  |  ___/ \  / ___|_   _| |
|  \| |/ _ \ \ /\ / / | __| '_ \ / _` | __|// __| | |_ / _ \ \___ \ | | | |
| |\  | (_) \ V  V /  | |_| | | | (_| | |_  \__ \ |  _/ ___ \ ___) || | |_|
|_| \_|\___/ \_/\_/    \__|_| |_|\__,_|\__| |___/ |_|/_/   \_\____/ |_| (_)

                             ...,?77??!~~~~!???77?<~....
                        ..?7`                           `7!..
                    .,=`          ..~7^`   I                  ?1.
       ........  ..^            ?`  ..?7!1 .               ...??7
      .        .7`        .,777.. .I.    . .!          .,7!
      ..     .?         .^      .l   ?i. . .`       .,^
       b    .!        .= .?7???7~.     .>r .      .=
       .,.?4         , .^         1        `     4...
        J   ^         ,            5       `         ?<.
       .%.7;         .`     .,     .;                   .=.
       .+^ .,       .%      MML     F       .,             ?,
        P   ,,      J      .MMN     F        6               4.
        l    d,    ,       .MMM!   .t        ..               ,,
        ,    JMa..`         MMM`   .         .!                .;
         r   .M#            .M#   .%  .      .~                 .,
       dMMMNJ..!                 .P7!  .>    .         .         ,,
       .WMMMMMm  ?^..       ..,?! ..    ..   ,  Z7`        `?^..  ,,
          ?THB3       ?77?!        .Yr  .   .!   ?,              ?^C
            ?,                   .,^.` .%  .^      5.
              7,          .....?7     .^  ,`        ?.
                `<.                 .= .`'           1
                ....dn... ... ...,7..J=!7,           .,
             ..=     G.,7  ..,o..  .?    J.           F
           .J.  .^ ,,,t  ,^        ?^.  .^  `?~.      F
          r %J. $    5r J             ,r.1      .=.  .%
          r .77=?4.    ``,     l ., 1  .. <.       4.,
          .$..    .X..   .n..  ., J. r .`  J.       `'
        .?`  .5        `` .%   .% .' L.'    t
        ,. ..1JL          .,   J .$.?`      .
                1.          .=` ` .J7??7<.. .;
                 JS..    ..^      L        7.:
                   `> ..       J.  4.
                    +   r `t   r ~=..G.
                    =   $  ,.  J
                    2   r   t  .;
              .,7!  r   t`7~..  j..
              j   7~L...$=.?7r   r ;?1.
               8.      .=    j ..,^   ..
              r        G              .
            .,7,        j,           .>=.
         .J??,  `T....... %             ..
      ..^     <.  ~.    ,.             .D
    .?`        1   L     .7.........?Ti..l
   ,`           L  .    .%    .`!       `j,
 .^             .  ..   .`   .^  .?7!?7+. 1
.`              .  .`..`7.  .^  ,`      .i.;
.7<..........~<<3?7!`    4. r  `          G%
                          J.` .!           %
                            JiJ           .`
                              .1.         J
                                 ?1.     .'
                                     7<..%

       _  ___    ____                        _ _
   _  / |/ _ \  | __ )  ___  _ __  _   _ ___| | |
 _| |_| | | | | |  _ \ / _ \| '_ \| | | / __| | |
|_   _| | |_| | | |_) | (_) | | | | |_| \__ \_|_|
  |_| |_|\___/  |____/ \___/|_| |_|\__,_|___(_|_)
|
TOTAL POINTS: 40 / 30
*/
// 
// -------REPLACE WITH YOUR RUN + TABLE --------

// You can write several different versions of your optimized function
// in this file and call one of them in the last function.

// context struct for thread workers to cooperate on column sums
typedef struct {
  int thread_id;                // logical id of thread, 0,1,2,...
  int thread_count;             // total threads working on summing
  matrix_t mat;                 // matrix to sum
  vector_t vec;                 // vector to place sums
  pthread_mutex_t *vec_lock;    // mutex to lock the vec before adding on results
} diagsums_context_t;

// worker thread defined below
void *diag_sums_worker(void *arg);

int sumdiag_VER1(matrix_t mat, vector_t vec, int thread_count) {
  if(vec.len != (mat.rows + mat.cols -1)){
    printf("sumdiag_base: bad sizes\n");
    return 1;
  }
  for(int i=0; i<vec.len; i++){ // initialize shared vector of diagonal sums
    VSET(vec,i,0); // to all 0s
  }

  // init a mutex to be used for threads to add on their local results
  pthread_mutex_t vec_lock;
  pthread_mutex_init(&vec_lock, NULL);

  pthread_t threads[thread_count]; // track each thread
  diagsums_context_t ctxs[thread_count]; // context for each trhead

  // loop to create threads
  for(int i=0; i<thread_count; i++){
    // set the fields of the context
    ctxs[i].thread_id = i;   
    ctxs[i].mat = mat;
    ctxs[i].thread_count = thread_count;
    ctxs[i].vec = vec;
    ctxs[i].vec_lock = &vec_lock;

    pthread_create(&threads[i], NULL,        // start worker thread to compute part of answer
                   diag_sums_worker,
                   &ctxs[i]);
  }
  
  // use a loop to join the threads
  for(int i = 0; i < thread_count; i++){
  pthread_join(threads[i], NULL);
  }
  
  pthread_mutex_destroy(&vec_lock); // get rid of the lock to avoid a memory leak
  return 0; 
}

// Threadless function, not in use (a function for noobs who can't use threads)
int sumdiag_VER2(matrix_t mat, vector_t vec, int thread_count) {
  if(vec.len != (mat.rows + mat.cols -1)){
    printf("sumdiag_base: bad sizes\n");
    return 1;
  }
  for(int i=0; i<vec.len; i++){                    // initialize vector of diagonal sums
    VSET(vec,i,0);                                 // to all 0s
  }
  vector_t local_vec = {.len = vec.len, .data = malloc(vec.len * sizeof(int))};

  for(int i=0; i<local_vec.len; i++){                    // initialize  local vector of diagonal sums
    VSET(local_vec,i,0);                                 // to all 0s
  }
  int d = 0; // indicates which element of the diagonal
  for(int i = mat.rows - 1; i >= 0; i--){ // iterate over lower diagonals
    d = mat.rows - 1 - i;
    for(int j = 0; j < mat.cols; j++){
      VSET(local_vec, d, VGET(local_vec, d) + MGET(mat, i, j));
      d++;
    }
  }
  for(int i = 0; i < vec.len; i++){
    VSET(vec, i, VGET(vec, i) + VGET(local_vec, i));
  }
  free(local_vec.data); // free the local vector
  return 0;
}

int sumdiag_OPTM(matrix_t mat, vector_t vec, int thread_count) {
  // call your preferred version of the function
  return sumdiag_VER1(mat, vec, thread_count);
  return 0;
}

// function for each thread to calcule its own diagonal sums and add them to the shared vector
void *diag_sums_worker(void *arg){ 
  // extract the parameters / "context" via a caste
  diagsums_context_t ctx = *((diagsums_context_t *) arg);
  //printf("Hello my thread ID is %d\n", ctx.thread_id);

  // extract the matrix and vector for the parameter context struct
  matrix_t mat = ctx.mat;
  vector_t vec = ctx.vec;

  // calculate how much work this thread should do and where its
  // begin/end rows are located. Leftover rows are handled by the last
  // thread.
  int rows_per_thread = mat.rows / ctx.thread_count;
  int beg_row = rows_per_thread * ctx.thread_id;
  int end_row = rows_per_thread * (ctx.thread_id+1);
  if(ctx.thread_id == ctx.thread_count-1){
    end_row = mat.rows;
  }
  
  // initialize  local vector of diagonal sums
  vector_t local_vec = {.len = vec.len, .data = malloc(vec.len * sizeof(int))};
  for(int i=0; i<local_vec.len; i++){ 
    VSET(local_vec,i,0); // to all 0s
  }
  // local varibles to try to speed up for bonus points
  int rows = mat.rows; 
  int columns = mat.cols;
  
  // calculate  diagonal sum into the local vector
  int d = beg_row; // indicates which element of the diagonal to work with
  for(int i = end_row - 1; i >= beg_row; i--){ // iterate over lower diagonals
    d = rows - 1 - i;
    for(int j = 0; j < columns; j++){
      VSET(local_vec, d, VGET(local_vec, d) + MGET(mat, i, j));
      d++;
    }
  }

  // lock the mutex to get controlled access to the shared
  // results vector and add on the local sum; locking prevents
  // multiple threads from simultaenously modifying the results which
  // may corrupt them.
  pthread_mutex_lock(ctx.vec_lock);

  // add the local vector to the shared vector
  for(int i = 0; i < vec.len; i++){
    VSET(vec, i, VGET(vec, i) + VGET(local_vec, i));
  }

  // unlock the mutex
  pthread_mutex_unlock(ctx.vec_lock);

  // free the local vector before ending
  free(local_vec.data);
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// REQUIRED: DON'T FORGET TO PASTE YOUR TIMING RESULTS FOR
// sumdiag_benchmark FROM AN ODD GRACE NODE AT THE TOP OF THIS FILE
////////////////////////////////////////////////////////////////////////////////
