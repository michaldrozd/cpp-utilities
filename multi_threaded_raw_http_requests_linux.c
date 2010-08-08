// Copyright (C) 2010 Michal Drozd
// All Rights Reserved

// It is important to note that the use of this program should be in compliance with the laws in your country.
// I cannot be held responsible for any actions taken as a result of using this program.

// C program in Linux which will send raw HTTP requests in each thread (number of threads = number of cores)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#define BUFFER_SIZE 1024

// Error function used for reporting issues
void error(const char* msg)
{
	perror(msg);
	exit(0);
}

// Struct to pass arguments to threads
struct thread_data
{
	int thread_id; // Thread ID number (just for reference)
	char* url; // URL to send GET request to
};

// Function to send GET request to specified URL
void* SendRequest(void* threadarg)
{
	struct thread_data* my_data;
	// Cast the void pointer to a struct pointer
	my_data = (struct thread_data*)threadarg;

	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[BUFFER_SIZE];

	// Set up the server address struct
	// Clear out the address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	// Get the port number, convert to an integer from a string
	portNumber = 80;
	// Create a network-capable socket
	serverAddress.sin_family = AF_INET;
	// Store the port number
	serverAddress.sin_port = htons(portNumber);

	// Get the DNS lookup for hostname
	serverHostInfo = gethostbyname(my_data->url);

	if (serverHostInfo == NULL)
	{
		fprintf(stderr, "CLIENT: ERROR, no such host\n");
		exit(0);
	}

	// Copy in the address
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket

	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	// Connect socket to address
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		error("CLIENT: ERROR connecting");
	}

	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array

	strcpy(buffer, "GET / HTTP/1.1\r\nHost: "); // Build GET request message with Host header and newline characters at end of each line
	strcat(buffer, my_data->url);
	strcat(buffer, "\r\nConnection: close\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36\r\nAccept-Language: en-US,en;q=0.8\r\nAccept-Encoding: gzip, deflate\r\nAccept: text/html");
	strcat(buffer, "\r\n");

	// Write to the server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0);

	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");

	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse

	while ((charsRead = recv(socketFD, buffer, BUFFER_SIZE - 1, 0)) > 0)
	{} // Read data from the socket until there is no more data left to read into buffer

	if (charsRead < 0) error("CLIENT: ERROR reading from socket");

	close(socketFD); // Close the socket

	pthread_exit((void*)my_data->thread_id); // Exit thread with ID number as return value
}

int main()
{
	int numCores = sysconf(_SC_NPROCESSORS_ONLN);
	/* Get number of cores available */
	printf("Number of cores available on this machine is %d \n", numCores);
	pthread_t threads[numCores];
	int rc;
	long t;
	void* status;      /* Create threads */     for (t = 0; t < numCores; t++)
	{
		printf("In main: creating thread %ld \n", t + 1);
		struct thread_data td[t];
		td[t].thread_id = t + 1;
		td[t].url = "www.google.com";
		rc = pthread_create(&threads[t], NULL, SendRequest, &td[t]);
		if (rc)
		{
			printf("ERROR return code from pthread create(): %d \n", rc);
			exit(-1);
		}
	}
	/* Wait for threads to finish */
	for (t = 0; t < numCores; t++)
	{
		rc = pthread_join((pthread_t)&threads[t], &status);
		if (rc)
		{
			printf("ERROR return code from pthread join(): %d \n", rc);
			exit(-1);
		}
		printf("Main completed join with thread %ld having a status of %ld \n", t + 1, status);
	}
	return 0;
}