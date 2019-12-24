#include "pch.h"
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <string>
#include <wchar.h>
#include <vector>
#include "..\Shared\constants.h"
using namespace std;





// https://stackoverflow.com/questions/6440715/how-to-pass-multiple-filenames-to-a-context-menu-shell-command
// We need wmain and wchar_t to support UNICODE characters.
int wmain(int argc, wchar_t **argv) {
	if (_setmode(_fileno(stdout), _O_U16TEXT) == -1) {
		cerr << "Error: Can't change stdout mode to unicode." << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}
	
	if (argc - 1 > MAXITEMCOUNT) {
		cerr << "Error: Exceeded maximum number of selected files (" << MAXITEMCOUNT << ")" << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}


	// Parsing names of selected items
	wstring rootDirPath; // Path of directory containing selected items.
	vector<wstring> itemNames; //  Filenames of seleceted items.
	rootDirPath = wstring(argv[1]);
	rootDirPath = rootDirPath.substr(0, rootDirPath.find_last_of(L"\\"));
	for (int i = 1; i < argc; i++)
		itemNames.push_back(wstring(wcsrchr(argv[i], L'\\') + 1));

	
	// Creating contents string
	// First line of contents is the number of selected items
	// Second line contains path of the directory of selected items
	int itemCount = argc - 1;
	wstring contents = to_wstring(itemCount) + L"\n" + rootDirPath + L"\n"; 
	for (vector<wstring>::const_iterator it = itemNames.begin(); it != itemNames.end(); it++)
		contents += *it + L"\n";



	// Event which notifies CopyApp that the ClipboarApp has acquired a handle 
	// to MMF (memory-mapped file). CopyApp cannot exit before this happens.
	// If an instance of ClipboardApp is already running the event will
	// already be in signaled state.
	HANDLE hAquiredMMFHandleEvent = CreateEvent( // Create or open
		NULL, // Default access flags
		TRUE, // Manual reset = true
		FALSE, // Initial state is nonsignaled
		EVENTNAME_AQUIREMMFHANDLE); // Name of the event 

	if (hAquiredMMFHandleEvent == NULL)
	{
		cerr << "Could not create or open MMF handle aquisition event" << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}



	// Start clipboard app.
	// Define ClipboardApp.exe startup and process info as default.
	STARTUPINFO siCPA;
	PROCESS_INFORMATION piCPA;
	ZeroMemory(&siCPA, sizeof(siCPA));
	siCPA.cb = sizeof(siCPA);
	ZeroMemory(&piCPA, sizeof(piCPA));

	// Start ClipboardApp.exe
	// Starting ClipboardApp.exe
	if (!CreateProcess(
		L"ClipboardApp.exe", // Program name.
		NULL, // Command line arguments (in this case uses L"ClipboardApp.exe")
		NULL,              // Process handle not inheritable
		NULL,              // Thread handle not inheritable
		FALSE,             // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,  // CREATE_NEW_CONSOLE  // Creation flags.
		NULL,              // Use parent's environment block
		NULL,              // Use parent's starting directory 
		&siCPA,            // Pointer to STARTUPINFO structure
		&piCPA)) {          // Pointer to PROCESS_INFORMATION structure
		cerr << "Error: Couldn't create ClipboardApp process." << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}
	


	// Populate memory mapped file.
	HANDLE hMapFile;
	LPWSTR pBuf;

	// Creating memory file mapping
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(wchar_t) * MAXCONTENTSSIZE,
		mmfName);

	if (hMapFile == NULL) {
		cerr << "Error: Couldn't create file mapping." << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}

	// Opening a view of memory mapped file
	pBuf = (LPWSTR) MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(wchar_t) * MAXCONTENTSSIZE);

	if (pBuf == NULL) {
		cerr << "Error: Couldn't create file view pBuf." << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}

	// Before writing to MMF we need to lock the mutex
	// Creates or opens the mutex.
	// Opening MMF mutex
	HANDLE hMMFMutex = CreateMutex(NULL, FALSE, MUTEXNAME_MMFMUTEX);
	if (hMMFMutex == NULL) {
		cerr << "Error: Failed to create or open MMF mutex." << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}
	if (WaitForSingleObject(hMMFMutex, INFINITE) != WAIT_OBJECT_0) {
		cerr << "Failed to acquire MMF mutex." << endl;
		return EXIT_FAILURE;
	}
	// Copying contents to memory mapped file
	CopyMemory(pBuf, contents.c_str(), contents.length() * sizeof(wchar_t));
	// Finished copying to memory mapped file
	ReleaseMutex(hMMFMutex);
	CloseHandle(hMMFMutex);


	// Wait until ClipboardApp aquires a handle to MMF 
	WaitForSingleObject(hAquiredMMFHandleEvent, INFINITE);
	// Event signaled 
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	CloseHandle(hAquiredMMFHandleEvent);
 	
	wcout << L"Robocopy Script Generated" << endl;
	wcout << L"Window will autoclose in 5 seconds" << endl;
	Sleep(5000);
	return 0;
}