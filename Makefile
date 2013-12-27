.PHONY: omp mpi

omp:
	gcc -Wall -fopenmp omp.c -o omp -lm

mpi:
	mpicc -Wall mpi.c -o mpi -lm
