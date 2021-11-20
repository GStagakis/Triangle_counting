CC=gcc
CILKCC=/usr/local/OpenCilk-9.0.1-Linux/bin/clang
CFLAGS=-O3

default: all

triangle_counting:
	$(CC) $(CFLAGS) -o triangle_counting triangle_counting.c
cilk_triangle_counting:
	$(CILKCC) $(CFLAGS) -o cilk_triangle_counting cilk_triangle_counting.c -fcilkplus
openmp_triangle_counting:
	$(CC) $(CFLAGS) -o openmp_triangle_counting openmp_triangle_counting.c -fopenmp
pthreads_triangle_counting:
	$(CC) $(CFLAGS) -o pthreads_triangle_counting pthreads_triangle_counting.c -lpthread	

.PHONY: clean

all: triangle_counting cilk_triangle_counting openmp_triangle_counting pthreads_triangle_counting

clean:
	rm -f triangle_counting cilk_triangle_counting openmp_triangle_counting pthreads_triangle_counting
