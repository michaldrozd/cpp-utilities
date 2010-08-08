// Copyright (C) 2010 Michal Drozd
// All Rights Reserved

// It is important to note that the use of this program should be in compliance with the laws in your country.
// I cannot be held responsible for any actions taken as a result of using this program.

// C program in Linux which will send curl HTTP requests in each thread (number of threads = number of cores)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_BUF 1024

_Noreturn void* thread_function(void* arg)
{
	char* url = (char*)arg;
	char cmd[MAX_BUF];
	sprintf(cmd, "curl -s %s", url);
	while (1)
	{
		system(cmd);
	}
}

int main(int argc, char** argv)
{

	if (argc != 2)
	{
		printf("Usage: ./a.out URL\n");
		exit(1);
	}
	else
	{
		// get number of cores available on the system
		int num_cores = get_nprocs();
		// create array of threads equal to number of cores available on the system
		pthread_t thread[num_cores];

		for (int i = 0; i < num_cores; i++)
		{
			// for each core create thread which will send http requests to specified input URL
			// create thread and pass input URL as argument to thread function
			pthread_create(&thread[i], NULL, thread_function, argv[1]);
		}

		for (int i = 0; i < num_cores; i++)
		{
			// wait for all threads to finish execution
			pthread_join(thread[i], NULL);
		}
	}
	return 0;
}