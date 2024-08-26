// clang -std=c11 -O3 -fopencilk reducer_ocilk_scalar.c
// CILK_NWORKERS=1 ./a.out
// CILK_NWORKERSの値は1,2,4,8,16,32,64,112と増やしていって性能確認

#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<cilk/cilk.h>

#define SIZE 1000000000L

// Timing function
double get_wall_time(){
  struct timeval time;
  if (gettimeofday(&time,NULL)){ 
    return 0;
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void zero_double(void *view) { *(double *)view = 0.0; }
void add_double(void *left, void *right)
    { *(double *)left += *(double *)right; }

void zero_long(void *view) { *(long *)view = 0.0; }
void add_long(void *left, void *right)
    { *(long *)left += *(long *)right; }

int main(void) {
    double cilk_reducer(zero_double, add_double) sumd=0;
    long cilk_reducer(zero_long, add_long) suml=0;
    double *a = (double *)malloc(SIZE * sizeof(double));
    long *b = (long *)malloc(SIZE * sizeof(long));
    for (long i = 0; i < SIZE; i++) { 
      a[i] = i;
      b[i] = i;
    }
    double start = get_wall_time();
    cilk_for (long i = 0; i < SIZE; i++) {
      sumd += a[i];
      suml += b[i];
    }
    double end = get_wall_time();
    printf("sumd = %f, suml = %ld\nTime: %f\n", sumd, suml, end - start);
    return 0;
    free(a); free(b);
}
