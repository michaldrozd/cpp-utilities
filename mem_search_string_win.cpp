// Copyright (C) 2010 Michal Drozd
// All Rights Reserved

// It is important to note that the use of this program should be in compliance with the laws in your country.
// I cannot be held responsible for any actions taken as a result of using this program.

// C++ code to search for given string (plain, UTF-8 or UNICODE) in given process in Windows.

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//----------------------------------------------------------------------------------------------------------------------
// Function: GetProcessIdByName()
// Description: Get process id by name. If multiple processes with same name are running, it returns the first one.
// If no process is found, it returns 0.
// Input: process_name - Name of the process to search for.
// Output: None.
// Return: Process id of the given process name.
//----------------------------------------------------------------------------------------------------------------------
DWORD GetProcessIdByName(const std::wstring& process_name)
{
	DWORD pid = 0;

	// Create a snapshot of all processes in the system.
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		std::wcerr << L"CreateToolhelp32Snapshot failed with error code " << GetLastError() << L"\n";
		return pid;
	}

	// Set the size of the structure before using it.
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process, and exit if unsuccessful
	if (!Process32First(hSnapshot, &pe32))
	{
		std::wcerr << L"Process32First failed with error code " << GetLastError() << L"\n";
		CloseHandle(hSnapshot); // clean the snapshot object
		return pid; // return 0 as no process is found with given name
	}
	else
	{
		// Process found, get its PID and exit loop
		do
		{
			// Check if this is the process we are looking for
			if (process_name == pe32.szExeFile)
			{
				// Found it!
				pid = pe32.th32ProcessID; // get its PID
				break;
			}
		} while (Process32Next(hSnapshot, &pe32));
	}

	// clean the snapshot object
	CloseHandle(hSnapshot);

	// return PID of given process name or 0 if not found
	return pid;
}



//----------------------------------------------------------------------------------------------------------------------
// Function: SearchStringInMemory()
// Description: Search for given string in memory of given process id
// Input: search_string - String to search for in memory of given process id
// pid - Process id to search for string in its memory
// Output: None
// Return: True if string is found in memory of given process id, false otherwise
//----------------------------------------------------------------------------------------------------------------------
bool SearchStringInMemory(const std::wstring& search_string, DWORD pid)
{
	bool found = false;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess == NULL)
	{
		std::wcerr << L"OpenProcess failed with error code " << GetLastError() << L"\n";
		return found;
	}

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	MEMORY_BASIC_INFORMATION mbi;
	for (LPVOID lpMem = si.lpMinimumApplicationAddress;
		 lpMem < si.lpMaximumApplicationAddress; lpMem = (LPVOID)((DWORD64)mbi.BaseAddress + (DWORD64)mbi.RegionSize))
	{
		if (VirtualQueryEx(hProcess, lpMem, &mbi, sizeof(mbi)) == 0)
		{
			// Failed to query memory region info
			// Skip this memory region and move to next one
			continue;
		}

		if ((mbi.State & MEM_COMMIT) && (mbi.Protect & PAGE_READWRITE))
		{
			// Check if this memory region is committed and readable
			// Allocate buffer of size equal to memory region size
			LPVOID buffer = malloc(mbi.RegionSize);

			// Number of bytes read from process memory
			SIZE_T bytesRead = 0;

			if (!ReadProcessMemory(hProcess, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead))
			{
				// Failed to read process memory
				// Free the allocated buffer
				free(buffer);

				// Skip this memory region and move to next one
				continue;
			}
			else
			{
				// Successfully read process memory into buffer
				auto* pBuffer = reinterpret_cast<wchar_t*>(buffer); // Cast the buffer pointer to wchar type pointer

				// Loop through each character in the buffer and check for search string match
				for (size_t i = 0; i < bytesRead / sizeof(wchar_t); ++i)
				{
					if (wcsstr(pBuffer, search_string.c_str()) != NULL ||
						strstr(reinterpret_cast<char*>(pBuffer), std::string(search_string.begin(), search_string.end()).c_str()) != nullptr ||
						MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<char*>(pBuffer), -1, NULL, 0) > 0)
					{
						// Found it!
						// Set found flag to true and exit loop
						found = true;
						break;
					}
				}
				// Free the allocated buffer
				free(buffer);
			}
		}
	}

	// Close the process handle
	CloseHandle(hProcess);

	// Return true or false based on whether search string is found or not in given process id's memory
	return found;

}



int main(int argc, char* argv[])
{
	assert((argc == 3));
	std::wstring searchString = std::wstring{ argv[1], strlen(argv[1]) };
	std::wstring processName = std::wstring{ argv[2], strlen(argv[2]) };
	DWORD pid = GetProcessIdByName(processName);
	assert((pid != 0));
	bool found = SearchStringInMemory(searchString, pid);
	std::cout << "Search string \"" << searchString << "\" is " << ((found) ? "" : "not ") << "found in process \""
			  << processName << "\"\n";
}