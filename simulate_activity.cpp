// Copyright (C) 2018 Michal Drozd
// All Rights Reserved

// This C++ program simulates user activity for various programs, such as online chats or call applications.
// By doing so, it keeps the currently logged-in user in the application appearing as "online and green".
// It is important to note that the use of this program should be in compliance with the laws in your country.
// I cannot be held responsible for any actions taken as a result of using this program.

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

// Function to simulate some activity in program by sending some keystrokes to it
void simulateActivity(HWND hwnd)
{
	// Generate random number between 1 and 5 to simulate different activity program
	srand(time(NULL));
	int randomNumber = rand() % 5 + 1;

	// Simulate activity based on generated random number
	if (randomNumber == 1)
	{
		// Simulate typing some text in program by sending keystrokes to it
		SendMessage(hwnd, WM_KEYDOWN, VK_SHIFT, 0); // Press Shift key down
		SendMessage(hwnd, WM_KEYDOWN, 0x41, 0); // Press A key down (A is capitalized because Shift key is pressed down)
		SendMessage(hwnd, WM_KEYUP, 0x41, 0); // Release A key up (A is capitalized because Shift key is pressed down)
		SendMessage(hwnd, WM_KEYUP, VK_SHIFT, 0); // Release Shift key up

	}
	else if (randomNumber == 2)
	{
		// Simulate typing some text in program by sending keystrokes to it
		SendMessage(hwnd, WM_KEYDOWN, VK_SHIFT, 0); // Press Shift key down
		SendMessage(hwnd, WM_KEYDOWN, 0x42, 0); // Press B key down (B is capitalized because Shift key is pressed down)
		SendMessage(hwnd, WM_KEYUP, 0x42, 0); // Release B key up (B is capitalized because Shift key is pressed down)
		SendMessage(hwnd, WM_KEYUP, VK_SHIFT, 0); // Release Shift key up

	}
	else if (randomNumber == 3)
	{
		// Simulate typing some text in program by sending keystrokes to it
		SendMessage(hwnd, WM_KEYDOWN, VK_SHIFT, 0); // Press Shift key down
		SendMessage(hwnd, WM_KEYDOWN, 0x43, 0); // Press C key down (C is capitalized because Shift key is pressed down)
		SendMessage(hwnd, WM_KEYUP, 0x43, 0); // Release C key up (C is capitalized because Shift key is pressed down)
		SendMessage(hwnd, WM_KEYUP, VK_SHIFT, 0); // Release Shift key up

	}
	else if (randomNumber == 4)
	{
		// Simulate typing some text in program by sending keystrokes to it
		SendMessage(hwnd, WM_KEYDOWN, VK_SHIFT, 0); // Press Shift key down
		SendMessage(hwnd, WM_KEYDOWN, 0x44, 0); // Press D key down (D is capitalized because Shift key is pressed down)
		SendMessage(hwnd, WM_KEYUP, 0x44, 0); // Release D key up (D is capitalized because Shift key is pressed down)
		SendMessage(hwnd, WM_KEYUP, VK_SHIFT, 0); // Release Shift Key Up

	}
	else if (randomNumber == 5)
	{
		// Simulate typing some text in program by sending keystrokes to it
		SendMessage(hwnd, WM_KEYDOWN, VK_SHIFT, 0); // Press Shift Key Down
		SendMessage(hwnd, WM_KEYDOWN, 0x45, 0); // Press E Key Down (E Is Capitalized Because Shift Key Is Pressed Down)
		SendMessage(hwnd, WM_KEYUP, 0x45, 0); // Release E Key Up (E Is Capitalized Because Shift Key Is Pressed Down)
		SendMessage(hwnd, WM_KEYUP, VK_SHIFT, 0); // Release Shift Key Up

	}
	else
	{
		cout << "Error: Random number generated was not between 1 and 5" << endl;
	}

	cout << "Simulated activity" << endl;
	return;
}

/* Function to get the process ID of the running process */
DWORD getProcessID()
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (_tcscmp(_T("Teams.exe"), entry.szExeFile) == 0)
			{
				CloseHandle(snapshot);
				return entry.th32ProcessID;
			}
		}
	}

	CloseHandle(snapshot);

	return 0;

}

/* Function to get the window handle of the running process */
HWND getWindowHandle(DWORD processID)
{
	HWND hwnd = NULL;

	do
	{
		hwnd = FindWindowEx(NULL, hwnd, NULL, NULL);
		DWORD lpdwProcessId;
		GetWindowThreadProcessId(hwnd, &lpdwProcessId);
		if (lpdwProcessId == processID)
		{
			return hwnd;
		}

	} while (hwnd != NULL);

	return NULL;
}

int main()
{
	cout << "Enter program name: ";
	getline(cin, programName);
	programName += ".exe";

	// Get handle of program by searching for it in running processes
	HWND hwnd = FindWindow(NULL, _T(programName.c_str()));
	if (hwnd == NULL)
	{
		cout << "Error: Program not found\n";
		return 1;
	}

	while (true)
	{
		simulateActivity(hwnd);
		Sleep(10000);
	}
	return 0;
}