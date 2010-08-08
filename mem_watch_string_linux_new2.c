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
#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s string\n", argv[0]);
		exit(1);
	}
	char* str = argv[1];
	char buf[BUF_SIZE];
	int fd = open("/proc/self/maps", O_RDONLY);
	if (fd == -1)
	{
		perror("open");
	}

	while (read(fd, buf, BUF_SIZE))
	{
	}
	close(fd);
	char* line = strtok(buf, "\n");
	while (line != NULL)
	{
		char* start = strtok(line, "-"); // start address of memory region in hexadecimal format
		char* end = strtok(NULL, " "); // end address of memory region in hexadecimal format

		// convert hexadecimal to decimal
		int start_addr = strtol(start, NULL, 16);
		int end_addr = strtol(end, NULL, 16);

		for (int i = start_addr; i < end_addr; i++)
		{
			// iterate over each byte in memory region
			if (*((char*)i) == str[0])
			{
				// check if first character of string is found in memory region
				int j;
				// check if entire string is found in memory region
				for (j = 0; j < strlen(str); j++)
				{
					// iterate over each character of string
					if (*((char*)i + j) != str[j])
					{
						// if any character does not match then break the loop and go to next memory region
						// break the loop and go to next memory region
						break;
					}
				}

				if (j == strlen(str))
				{
					// if entire string is found then print the address where it is found and exit the program with success status code 0
					// print the address where string is found and exit the program with success status code 0
					printf("%p\n", i);
					exit(0);
				}
			}
		}

		// get next line from maps file which contains information about next memory region
		line = strtok(NULL, "\n");

	}

	// return failure status code 1 as no occurence of given string was found in process's entire virtual memory space
	return 1;
}