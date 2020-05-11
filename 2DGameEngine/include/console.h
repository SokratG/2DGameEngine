#pragma once
#include "Window.h"


HANDLE CreateConsole(VOID)
{
	static HANDLE _console;
	// detach from the current console window
	// if launched from a console window, that will still run waiting for the new console (below) to close
	FreeConsole();
	// create a separate new console window
	AllocConsole();
	// attach the new console to this application's process
	AttachConsole(GetCurrentProcessId());

	// reopen the std I/O streams to redirect I/O to the new console
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	freopen("CON", "r", stdin);

	_console = GetStdHandle(STD_OUTPUT_HANDLE);

	return _console;
}

VOID SetColorConsole(_In_ HANDLE& hConsole, _In_ WORD color)
{
	SetConsoleTextAttribute(hConsole, color);
}

VOID ResetConsoleColor(_In_ HANDLE& hConsole)
{
	SetConsoleTextAttribute(hConsole, CDEFUALT);
}
