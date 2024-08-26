// clang -std=c11 -O3 -fopencilk reducer_ocilk_array.c
// CILK_NWORKERS=1 ./a.out
// CILK_NWORKERSの値は1,2,4,8,16,32,64,112と増やしていって性能確認

#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<cilk/cilk.h>

#define SIZE 1000
#define NVECTOR 1000000

// Timing function
double get_wall_time(){
  struct timeval time;
  if (gettimeofday(&time,NULL)){ 
    return 0;
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void zero_doubles(void *view) {
  *(double **)view = (double *)calloc(SIZE, sizeof(double));
}
void add_doubles(void *left, void *right) {
  for (size_t i=0; i<SIZE; i++) {
    (*(double **)left)[i] += (*(double **)right)[i];
  }
  free(*(double **)right);
}
void zero_longs(void *view) {
  *(long **)view = (long *)calloc(SIZE, sizeof(long));
}
void add_longs(void *left, void *right) {
  for (size_t i=0; i<SIZE; i++) {
    (*(long **)left)[i] += (*(long **)right)[i];
  }
  free(*(long **)right);
}

int main(void) {
    double *cilk_reducer(zero_doubles, add_doubles) sumd= calloc(SIZE, sizeof(double));
    long *cilk_reducer(zero_longs, add_longs) suml= calloc(SIZE, sizeof(long));
    double (*a)[SIZE] = malloc(NVECTOR*SIZE*sizeof(double));
    long (*b)[SIZE] = malloc(NVECTOR*SIZE*sizeof(long));
    for (long i = 0; i < NVECTOR; i++) { 
      for (long j = 0; j < SIZE; j++) { 
        a[i][j] = i;
        b[i][j] = i;
      }
    }
    double start = get_wall_time();
    cilk_for (long i = 0; i < NVECTOR; i++) {
      for (long j = 0; j < SIZE; j++) { 
        sumd[j] += a[i][j];
        suml[j] += b[i][j];
      }
    }
    double end = get_wall_time();
    printf("sumd[0] = %f, suml[0] = %ld\nTime: %f\n", sumd[0], suml[0], end - start);
    // error check
    for (size_t i=0; i<SIZE-1; i++) {
      if (sumd[i] != sumd[i+1]) {
        printf("error: sumd[%zu] = %f, sumd[%zu] = %f\n", i, sumd[i], i+1, sumd[i+1]);
        exit(1);
      }
      if (suml[i] != suml[i+1]) {
        printf("error: suml[%zu] = %ld, suml[%zu] = %ld\n", i, suml[i], i+1, suml[i+1]);
        exit(1);
      }
    }
    return 0;
    free(a); free(b);
    free(sumd); free(suml);
}
