// Copyright (C) 2010 Michal Drozd
// All Rights Reserved

// It is important to note that the use of this program should be in compliance with the laws in your country.
// I cannot be held responsible for any actions taken as a result of using this program.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

int main(int argc, char* argv[])
{

	if (argc != 3)
	{
		printf("Usage: %s [pid] [string]\n", argv[0]);
		return 1;
	}
	int pid = atoi(argv[1]); // convert string to integer and store it in pid variable
	char* str = argv[2]; // store string to be searched in str variable
	char path[100]; // create path variable to store path of file containing memory map of process with given pid
	sprintf(path, "/proc/%d/maps", pid); // create path string and store it in path variable
	FILE* fp = fopen(path, "r"); // open file with given path in read mode and store its pointer in fp variable

	// check if file is opened successfully or not by checking value of fp pointer (NULL indicates error)
	if (fp == NULL)
	{
		perror("Error");
	}
	else
	{
		char line[100]; // create line variable to store each line from file containing memory map of process with given pid
		while (fgets(line, sizeof(line), fp))
		{
			// read each line from file containing memory map of process with given pid and store it in line variable until end of file is reached

			// extract starting address from each line and store it in start_addr variable by using strtok function
			// which returns first token from string passed as argument (here line) by splitting it using delimiter passed as second argument ("-" here)
			char* start_addr = strtok(line, "-");
			// extract ending address from each line and store it in end_addr variable by using strtok function
			// which returns next token from string passed as argument (here NULL here because we want next token
			// after first one which was already extracted above) by splitting it using delimiter passed as second argument (" " here)
			char* end_addr = strtok(NULL, " ");

			// convert starting and ending address stored as string into unsigned long integer and store it in start_address variable
			// by using strtoul function which converts string passed as first argument into unsigned long integer
			// and stores result into second argument (here NULL because we don't want to store result anywhere else)
			// by using base passed as third argument (16 here because addresses are hexadecimal numbers)
			unsigned long start_address = strtoul(start_addr, NULL, 16);

			unsigned long end_address = strtoul(end_addr, NULL, 16);

			// calculate size of memory region between starting address and ending address by subtracting starting address from ending address and storing result in size variable
			unsigned long size = end_address - start_address;

			// map memory region between starting address and ending address with read write execute permissions on shared anonymous memory region
			// fixed at given starting address by calling mmap function which maps files or devices into memory and returns a pointer to the mapped area or MAP FAILED on error
			void* map = mmap((void*)start_address, size,
				PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_FIXED
													| MAP_ANONYMOUS, -1, 0);

			if (map == MAP_FAILED)
			{
				// check if mapping failed or not by checking value returned by mmap function i.e., value stored in map pointer (MAP FAILED indicates error while any other value indicates successfull mapping)
				perror("Error");
			}
			else
			{
				char* ptr;
				/* search for occurence of string stored in str variable within mapped memory region between starting address and ending address with size equal to difference between ending address and starting address */
				ptr = memmem((void*)start_address, size, str, strlen(str));
				/* check if occurence found or not i.e., check value returned by memmem function i.e., value stored in ptr pointer */
				if (ptr != NULL)
				{
					printf("%s\n", ptr);
				}
			}
		}
	}
	return 0;
}