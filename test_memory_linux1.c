// Copyright (C) 2010 Michal Drozd
// All Rights Reserved

// It is important to note that the use of this program should be in compliance with the laws in your country.
// I cannot be held responsible for any actions taken as a result of using this program.

// Test memory in Linux

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#define SIZE 1000000 // 1 million bytes = 1 MB

int main(int argc, char *argv[]) {

	// Allocate memory for the array of size SIZE (1 MB)
	char *array = malloc(SIZE);

	// Fill the array with 'a' characters (97 in ASCII)
	memset(array, 97, SIZE);

	// Print out the first 10 elements of the array to make sure it worked correctly
	for (int i = 0; i < 10; i++) {
		printf("%c", array[i]);
	}

	printf("\n");

	// Get the current process ID and print it out for reference later on in top output
	pid_t pid = getpid();

	printf("PID: %d\n", pid);

	// Get the current time and print it out for reference later on in top output
	struct timeval tv;

	gettimeofday(&tv, NULL);

	printf("Time: %ld\n", tv.tv_sec);

	// Get the current memory usage and print it out for reference later on in top output
	struct rusage usage;

	getrusage(RUSAGE_SELF, &usage);

	printf("Memory Usage: %ld\n", usage.ru_maxrss);

	// Wait for user input before exiting program so that we can see the memory usage in top output
	printf("Press enter to exit program...\n");

	getchar();

	return 0;
}