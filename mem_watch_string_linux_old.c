// Copyright (C) 2010 Michal Drozd
// All Rights Reserved

// It is important to note that the use of this program should be in compliance with the laws in your country.
// I cannot be held responsible for any actions taken as a result of using this program.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>

int main(int argc, char* argv[])
{

	if (argc != 3)
	{
		printf("Usage: %s <process_name> <string_to_find>\n", argv[0]);
		exit(1);
	}

	char* proc = argv[1];
	char* str = argv[2];

	pid_t pid;

	if ((pid = fork()) == 0)
	{
		// child process

		if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
		{
			// trace child process itself (PTRACE_TRACEME)
			perror("ptrace");
		}
		// execute process to be traced (given as input)
		execlp(proc, proc, NULL);
	}
	else if (pid > 0)
	{
		// parent process
		int status;
		wait(&status); // wait for child process to stop or exit

		if (WIFEXITED(status))
		{
			// check if child exited normally or not?
			// print exit status of child process if it exited normally
			printf("Child exited with status: %d\n", WEXITSTATUS(status));
			// exit normally from parent process too if child exited normally
			exit(0);
		}
		else if (WIFSTOPPED(status))
		{
			// check if child stopped or not? If yes then continue tracing it...
			while (1)
			{
				// infinite loop to trace the child process until it exits or we stop tracing it manually by pressing Ctrl+C...
				// structure to store registers values of traced process at any given time...
				struct user_regs_struct regs;
				// get registers values of traced process and store them in 'regs' structure...
				ptrace(PTRACE_GETREGS, pid, NULL, &regs);
				// get instruction pointer (EIP) value from 'regs' structure...
				unsigned long addr = regs.rip;
				// allocate memory to store memory contents of traced process at any given time...
				unsigned char* mem = (unsigned char*)malloc(strlen(str));
				ptrace(PTRACE_PEEKTEXT, pid, addr, mem); // read memory contents of traced process at address 'addr' and store them in 'mem' variable...
				if (strcmp(mem, str) == 0)
				{
					// compare memory contents of traced process with string to be found (given as input)...
					printf("String found at address: %lu\n", addr); // print address where string was found...
				}
				free(mem); // free allocated memory...

				ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL); // continue tracing child process by single stepping it...

				wait(&status); // wait for child process to stop or exit...

				if (WIFEXITED(status))
				{
					// check if child exited normally or not?
					printf("Child exited with status: %d\n", WEXITSTATUS(status)); // print exit status of child process if it exited normally
					exit(0); // exit normally from parent process too if child exited normally
				}
				else if (WIFSTOPPED(status))
				{
					continue;
				}
				else
				{
					printf("Child stopped with signal: %d\n", WSTOPSIG(status)); // print signal which stopped the child process if it was stopped by a signal...
				}
			}
		}
		else
		{
			printf("Child stopped with signal: %d\n", WSTOPSIG(status)); // print signal which stopped the child process if it was stopped by a signal...
			exit(-1); // exit abnormally from parent process too if child was stopped by a signal...
		}
	}
	else
	{
		perror("fork");
	}

	return 0;
}