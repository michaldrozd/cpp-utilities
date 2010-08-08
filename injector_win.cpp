// Copyright (C) 2010 Michal Drozd
// All Rights Reserved

// This C++ program can be used to test your Windows program against code injection.
// The use of this software is subject to the laws and regulations of the country in which it is being used.
// The developer of this software is not responsible for any illegal or unauthorized use of the program!
// By using this software, you agree to use it in compliance with all applicable laws and regulations!

// Code injection can be useful in a few legal activities, such as:
// - Debugging and troubleshooting: Code injection can be used to debug and troubleshoot running processes by injecting code to collect information about the state of the program or to modify its behavior to diagnose and fix issues.
// - Automated testing: To automate testing by injecting code into your running process to simulate user inputs and interactions, and to verify that the program behaves correctly.
// - Security research: To identify and exploit vulnerabilities in your own software by injecting code to test for weaknesses and to demonstrate how they can be exploited.
// - Memory management: To manipulate the memory of your running process to optimize the use of system resources and to improve performance.

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>
using namespace std;

// Function to inject given assembly in hexadecimal format to given process at specified address
void inject(const string& processName, const string& address, const string& assembly) {
	// Get process ID
	DWORD processID = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	process.dwSize = sizeof(process);
	if (Process32First(snapshot, &process)) {
		do {
			if (!strcmp(process.szExeFile, processName.c_str())) {
				processID = process.th32ProcessID;
			}
		} while (Process32Next(snapshot, &process));
	}

	// Open process with all access
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

	// Allocate memory for assembly in target process and write assembly to it
	LPVOID addr = (LPVOID)strtoul(address.c_str(), NULL, 16); // Convert address string to hexadecimal address
	LPVOID asmAddr = VirtualAllocEx(hProcess, addr, assembly.length() / 2, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); // Allocate memory for assembly in target process at specified address
	WriteProcessMemory(hProcess, asmAddr, assembly.c_str(), assembly.length() / 2, NULL); // Write assembly to allocated memory

	// Create remote thread at entry point of allocated memory to execute assembly in target process
	CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)asmAddr, NULL, 0, NULL);

	// Free allocated memory and close handle to process
	VirtualFreeEx(hProcess, asmAddr, 0, MEM_RELEASE);
}

int main() {

	// Read assembly from file and store it in a string variable
	ifstream file("assembly.txt"); // Open file containing assembly in hexadecimal format (one byte per line)
	stringstream buffer; // Create string stream to store contents of file in a string variable
	buffer << file.rdbuf(); // Read contents of file into string stream buffer

	// Convert string stream buffer into a vector of strings containing each byte of the assembly in hexadecimal format
	vector<string> bytes; // Vector to store each byte of the assembly in hexadecimal format
	istream_iterator<string> start(buffer), end; // Create iterator to iterate through each line of the string stream buffer
	copy(start, end, back_inserter(bytes)); // Copy each line of the string stream buffer into the vector

	// Convert vector of strings containing each byte of the assembly in hexadecimal format into a single string containing the entire assembly in hexadecimal format
	stringstream ss; // Create string stream to store entire assembly in hexadecimal format
	for (auto& byte : bytes) { ss << byte; } // Append each byte of the assembly to the string stream

	inject("my_own_program.exe", "00400000", ss.str()); // Inject given assembly into given process at specified address

	return 0;
}