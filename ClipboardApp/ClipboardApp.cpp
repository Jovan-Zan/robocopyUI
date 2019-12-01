#include <iostream>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <wchar.h>
#include "..\Shared\constants.h"
using namespace std;

wchar_t contents[MAXCONTENTSSIZE + 1];


int wmain(int argc, wchar_t** argv) {
	if (_setmode(_fileno(stdout), _O_U16TEXT) == -1) {
		cerr << "Error: Can't change stdout mode to unicode." << endl;
		return EXIT_FAILURE;
	}

	// Create or open mutex.
	wcout << L"Opening mutex in ClipboardApp..." << endl;
	HANDLE hCPAMutex = OpenMutex(SYNCHRONIZE, FALSE, MUTEXNAME);
	if (hCPAMutex == NULL) {
		cerr << "Error: Failed to open clipboardapp mutex." << endl;
		Sleep(3000);
		return EXIT_FAILURE;
	}

	DWORD dwTryAcquireMutex = WaitForSingleObject(hCPAMutex, 0);
	if (dwTryAcquireMutex != WAIT_OBJECT_0) {
		cerr << "Couldn't acquire mutex in ClipboardApp." << endl;
		return EXIT_FAILURE;
	}



	// Get the handle to memory mapped file in order to keep it alive and in memory.
	wcout << L"Openning file mapping..." << endl;
	HANDLE hMapFile = OpenFileMapping(
		FILE_MAP_READ,         // read access
		FALSE,                 // do not inherit the name
		mmfName);              // name of mapping object

	if (hMapFile == NULL) {
		cerr << "Error: Couldn't open file mapping." << endl;
		return EXIT_FAILURE;
	}

	wcout << L"Creating a view of memory mapped file..." << endl;
	LPWSTR pBuf = (LPWSTR) MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_READ, // read/write permission
		0,
		0,
		MAXCONTENTSSIZE);

	if (pBuf == NULL)
	{
		cerr << "Error: Couldn't open file map for reading." << endl;
		return EXIT_FAILURE;
	}

	wcout << L"Getting contests of memory mapped file..." << endl;
	wcout << pBuf;

	wcout << "FINISHED" << endl;

	Sleep(MAXRUNNINGTIME);

	CloseHandle(hMapFile);
	ReleaseMutex(hCPAMutex);
	CloseHandle(hCPAMutex);
	return 0;
}