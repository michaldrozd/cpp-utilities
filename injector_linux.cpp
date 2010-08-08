// Copyright (C) 2010 Michal Drozd
// All Rights Reserved

// This C++ program can be used to test your Linux program against code injection.
// The use of this software is subject to the laws and regulations of the country in which it is being used.
// The developer of this software is not responsible for any illegal or unauthorized use of the program!
// By using this software, you agree to use it in compliance with all applicable laws and regulations!

// Code injection can be useful in a few legal activities, such as:
// - Debugging and troubleshooting: Code injection can be used to debug and troubleshoot running processes by injecting code to collect information about the state of the program or to modify its behavior to diagnose and fix issues.
// - Automated testing: To automate testing by injecting code into your running process to simulate user inputs and interactions, and to verify that the program behaves correctly.
// - Security research: To identify and exploit vulnerabilities in your own software by injecting code to test for weaknesses and to demonstrate how they can be exploited.
// - Memory management: To manipulate the memory of your running process to optimize the use of system resources and to improve performance.

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// Function to convert hex string to decimal number (unsigned long long)
unsigned long long hexToDec(const string& hexStr)
{
	unsigned long long decNum;
	stringstream ss;
	ss << std::hex << hexStr; // convert hex string to stream of hex numbers
	ss >> decNum; // convert stream of hex numbers to decimal number (unsigned long long) and store in decNum variable
	return decNum; // return decimal number (unsigned long long) from function call
}

// Function to convert decimal number (unsigned long long) to hex string
string decToHex(unsigned long long decNum)
{
	stringstream ss;  // create a stringstream object named ss
	ss << std::hex
	   << decNum; // insert the input decimal number into the stringstream object named ss and convert it to a stream of hex numbers
	return ss.str(); // return the converted hex string from function call
}

// Function to convert assembly code to machine code (hex string) using objdump command in Linux terminal and store it in a vector of strings named machineCodeVector
// pass by reference for machineCodeVector so that it can be modified inside the function and the changes will be reflected outside the function as well
void assemblyToMachineCode(const string& assembly, vector<string>& machineCodeVector)
{
	// Create a temporary file named tempFile with .s extension which contains the input assembly code and store it in /tmp directory in Linux system

	// create a temporary file named tempFile with .s extension in /tmp directory in Linux system
	ofstream tempFile("/tmp/tempFile.s");

	// check if the temporary file is open or not
	if (tempFile.is_open())
	{
		// write the input assembly code into the temporary file named tempFile
		tempFile << assembly;

		// close the temporary file named tempFile
		tempFile.close();

		// compile the temporary file named tempFile with .s extension using gcc compiler and generate an object file named tempFile with no extension in /tmp directory in Linux system
		system("gcc -c /tmp/tempFile.s -o /tmp/tempFile");

		// disassemble the object file named tempFile with no extension using objdump command and generate an output file named tempFile2 with no extension in /tmp directory in Linux system
		system("objdump -d /tmp/tempFile > /tmp/tempFile2");

		// open the output file named tempFile2 with no extension for reading purpose only
		ifstream tempFile2("/tmp/tempFile2");

		// check if the output file is open or not
		if (tempFile2.is_open())
		{
			// create a string variable named line which will store each line of text from output file named tempFile2 one by one
			string line;

			// read each line of text from output file named tempFile2 one by one until end of file is reached
			while (getline(tempFile2, line))
			{
				// check if current line of text from output file named tempFile2 starts with "         :" or not
				if (line.substr(0, 11) == "          :")
				{
					// initialize i variable with value 11 which will be used as index for current line of text from output file named tempFile2
					int i = 11;

					// loop through each character of current line of text from output file named tempFile2 until null character is reached
					while (line[i] != 0)
					{
						// check if current character is not space or not
						if (line[i] != 32)
						{
							// initialize j variable with value i + 1 which will be used as index for current character of current line of text from output file named tempFile2
							int j = i + 1;

							// loop through each character of current line of text from output file named tempFile2 until space or null character is reached starting from index j
							while (line[j] != 32 && line[j] != 0)
							{
								// increment j by 1 after each iteration of while loop so that next character can be checked in next iteration of while loop
								j++;
							}
							// extract substring from current line of text from output file named tempFile2 starting at index i and ending at index j - 1 and push it into machineCodeVector vector as a new element after each iteration of while loop so that all machine codes can be stored in machineCodeVector vector one by one
							machineCodeVector.push_back(line.substr(i, j - i));
							// set i equal to j + 1 after each iteration of while loop so that next machine code can be extracted in next iteration of while loop starting at index i + 1
							i = j + 1;
						}
						else
						{
							// increment i by 1 after each iteration of while loop so that next character can be checked in next iteration of while loop
							i++;
						}
					}
				}
			}
			// close the output file named tempFile2 after reading all lines from it one by one until end of file is reached
			tempFile2.close();
		}
		else
		{
			// if the output file named tempFile2 is not open
			cout << "Unable to open file" << endl; // display error message
		}
	}
	else
	{
		// if the temporary file named tempFile is not open
		cout << "Unable to open file" << endl; // display error message
	}
}

// Main function
void inject(const string& processName, const string& address, const string& assembly)
{
	// create a child process using fork() system call and store the process id of child process in pid variable
	pid_t pid = fork();
	// check if the value of pid variable is 0 or not
	if (pid == 0)
	{
		// trace the child process using ptrace() system call
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		// execute ls command in child process using execl() system call
		execl("/bin/ls", "ls", NULL);
	}
	else
	{
		// if the value of pid variable is not 0
		// create an integer variable named status which will store the status of child process
		int status;
		// wait for child process to stop and store its status in status variable using waitpid() system call
		waitpid(pid, &status, 0);
		// set options for tracing the child process using ptrace() system call so that it can be killed when it exits
		ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_EXITKILL);
		// create a vector of strings named machineCodeVector which will store all machine codes one by one generated from input assembly code
		vector<string> machineCodeVector;
		// call assemblyToMachineCode() function to convert input assembly code to machine code (hex string) and store it in machineCodeVector vector one by one
		assemblyToMachineCode(assembly, machineCodeVector);
		// convert input address from hex string to decimal number and store it in addressDec variable
		unsigned long long addressDec = hexToDec(address);
		// loop through each element of machineCodeVector vector one by one until last element is reached starting from index 0
		for (int i = 0; i < machineCodeVector.size(); i++)
		{
			// Convert current element of machineCodeVector vector from hex string to decimal number
			// and store it in machineCodeDec variable after each iteration of for loop
			// so that all elements of machineCodeVector vector can be converted from hex string to decimal number one by one and stored in machineCodeDec variable one by one
			unsigned long long machineCodeDec = hexToDec(machineCodeVector[i]);

			// This one is the most IMPORTANT call here!
			// Write current element of machineCodeVector vector into memory location pointed by addressDec + i * 4 after each iteration of for loop
			// so that all elements of machineCodeVector vector can be written into memory locations pointed by addressDec + i * 4 one by one starting from index 0 until last element is reached
			ptrace(PTRACE_POKETEXT, pid, addressDec + i * 4, machineCodeDec);
		}
	}
}

int main()
{
	inject("ls", "0x4004d0", "mov eax, 1\n mov ebx, 2\n add eax, ebx\n");

	return 0;
}