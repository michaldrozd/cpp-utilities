// Copyright (C) 2010 Michal Drozd
// All Rights Reserved

// It is important to note that the use of this program should be in compliance with the laws in your country.
// I cannot be held responsible for any actions taken as a result of using this program.

// NVIDIA Cuda benchmark for Linux

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cuda_runtime.h>
#include "cuda_test_kernel.h"

// Define the number of threads per block and the number of blocks in the grid
#define NUM_THREADS 1024
#define NUM_BLOCKS  1024

int main(int argc, char **argv) {

	// Allocate host memory for the matrices A and B and C (the result matrix)
	int *A = (int *)malloc(NUM_BLOCKS * NUM_THREADS * sizeof(int));
	int *B = (int *)malloc(NUM_BLOCKS * NUM_THREADS * sizeof(int));
	int *C = (int *)malloc(NUM_BLOCKS * NUM_THREADS * sizeof(int));

	// Initialize the host input matrices A and B with random values
	for (int i = 0; i < NUM_BLOCKS * NUM_THREADS; ++i) {
		A[i] = rand() % 100;  // Random number between 0 and 99
		B[i] = rand() % 100;  // Random number between 0 and 99
	}

	// Allocate device memory for matrices A, B, and C
	int *dA, *dB, *dC;

	// Allocate device memory for matrix A, B, C
	cudaMalloc((void **)&dA, NUM_BLOCKS*NUM_THREADS*sizeof(int));
	cudaMalloc((void **)&dB, NUM_BLOCKS*NUM_THREADS*sizeof(int));
	cudaMalloc((void **)&dC, NUM_BLOCKS*NUM_THREADS*sizeof(int));

	// Copy host matrices A and B to device matrices dA and dB respectively on the GPU device
	// Copy from host to device memory for matrix A
	cudaMemcpy(dA, A, NUM_BLOCKS*NUM_THREADS*sizeof(int), cudaMemcpyHostToDevice);
	// Copy from host to device memory for matrix B
	cudaMemcpy(dB, B, NUM_BLOCKS*NUM_THREADS*sizeof(int), cudaMemcpyHostToDevice);

	// Launch the kernel on the GPU device
	dim3 dimGrid(NUM_BLOCKS, 1, 1);
	dim3 dimBlock(NUM_THREADS, 1, 1);

	// Call the kernel function with NUM_BLOCKS blocks and NUM_THREADS threads per block
	cuda_test_kernel<<<dimGrid, dimBlock>>>(dA, dB, dC);

	// Copy the result matrix C from device memory to host memory
	cudaMemcpy(C, dC, NUM_BLOCKS*NUM_THREADS*sizeof(int), cudaMemcpyDeviceToHost);

	// Free device memory
	cudaFree(dA);
	cudaFree(dB);
	cudaFree(dC);

	// Print the result matrix C on the host machine (CPU)
     for (int i = 0; i < NUM_BLOCKS * NUM_THREADS; ++i) {
         printf("%d\n", C[i]);
     }

	free(A);
	free(B);
	free(C);

	return 0;
}