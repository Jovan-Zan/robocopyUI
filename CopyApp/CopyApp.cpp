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
		return EXIT_FAILURE;
	}
	
	if (argc - 1 > MAXITEMCOUNT) {
		cerr << "Error: Exceeded maximum number of selected files (" << MAXITEMCOUNT << ")" << endl;
		return EXIT_FAILURE;
	}

	wcout << L"Number of arguments = " << argc << endl;
	wcout << "Arguments: " << endl;
	for (int i = 0; i < argc; i++)
		wcout << "argv[" << i << "] = " << argv[i] << endl;
	wcout << endl;


	HANDLE hMapFile;
	LPWSTR pBuf;
	wstring rootDirPath;
	vector<wstring> itemPaths;

	rootDirPath = wstring(argv[1]);
	rootDirPath = rootDirPath.substr(0, rootDirPath.find_last_of(L"\\"));
	wcout << "Root dir path = " << rootDirPath << endl;

	for (int i = 1; i < argc; i++)
		itemPaths.push_back(wstring(wcsrchr(argv[i], L'\\') + 1));

	wcout << "Items: " << endl;
	for (int i = 0; i < itemPaths.size(); i++)
		wcout << "itemPaths[" << i << "] = " << itemPaths[i] << endl;
	
	wcout << L"Creating contents string..." << endl;
	wstring contents = rootDirPath + L"\n";
	for (vector<wstring>::const_iterator it = itemPaths.begin(); it != itemPaths.end(); it++)
		contents += *it + L"\n";

	wcout << L"Contents = " << endl << contents << endl << endl;;


	// Populate memory mapped file.
	wcout << L"Creating memory file mapping..." << endl;
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(wchar_t) * MAXCONTENTSSIZE,
		mmfName);

	if (hMapFile == NULL) {
		cerr << "Error: Couldn't create file mapping." << endl;
		return EXIT_FAILURE;
	}

	wcout << L"Opening a view of memory mapped file..." << endl;
	pBuf = (LPWSTR) MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(wchar_t) * MAXCONTENTSSIZE);

	if (pBuf == NULL) {
		cerr << "Error: Couldn't create file view pBuf." << endl;
		return EXIT_FAILURE;
	}
	
	wcout << L"Copying contents to memory mapped file..." << endl;
	CopyMemory(pBuf, contents.c_str(), contents.length() * sizeof(wchar_t));
	wcout << L"Finished copying to memory mapped file." << endl << endl;



	// Start clipboard app.

	// Create or open mutex.
	wcout << L"Opening mutex..." << endl;
	HANDLE hCPAMutex = CreateMutex(NULL, FALSE, MUTEXNAME);
	if (hCPAMutex == NULL) {
		cerr << "Error: Failed to create or open clipboardapp mutex." << endl;
		Sleep(3000);
		return EXIT_FAILURE;
	}

	// If mutex is already acquired that means an instance of
	// ClipboardApp.exe is already running.
	DWORD dwWaitRes = WaitForSingleObject(hCPAMutex, 0);

	// Mutex is signaled and we have ownership of mutex.
	if (dwWaitRes == WAIT_OBJECT_0) {
		wcout << L"Mutex IS signaled." << endl;

		// Define ClipboardApp.exe startup and process info as default.
		STARTUPINFO siCPA;
		PROCESS_INFORMATION piCPA;
		ZeroMemory(&siCPA, sizeof(siCPA));
		siCPA.cb = sizeof(siCPA);
		ZeroMemory(&piCPA, sizeof(piCPA));

		// Start ClipboardApp.exe
		wcout << L"Starting ClipboardApp.exe ..." << endl;
		ReleaseMutex(hCPAMutex);
		if (!CreateProcess(
			L"ClipboardApp.exe", // Program name.
			NULL, // Command line arguments (in this case uses L"ClipboardApp.exe")
			NULL,              // Process handle not inheritable
			NULL,              // Thread handle not inheritable
			FALSE,             // Set handle inheritance to FALSE
			CREATE_NEW_CONSOLE, //CREATE_NO_WINDOW,  // Creation flags.
			NULL,              // Use parent's environment block
			NULL,              // Use parent's starting directory 
			&siCPA,            // Pointer to STARTUPINFO structure
			&piCPA)) {          // Pointer to PROCESS_INFORMATION structure
			cerr << "Error: Couldn't create ClipboardApp process." << endl;
			return EXIT_FAILURE;
		}
	}
	else {
		wcout << L"Mutex IS NOT signaled. An instance of ClipboardApp is already running." << endl;
	}


	wcout << L"THE END" << endl;
	Sleep(60000);

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	CloseHandle(hCPAMutex);
 	return 0;
}