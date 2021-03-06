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
		Sleep(5000);
		return EXIT_FAILURE;
	}



	// Creates or opens the mutex.
	// Opening mutex
	HANDLE hCBAMutex = CreateMutex(NULL, FALSE, MUTEXNAME_SINGLECLIPBOARDAPP);
	if (hCBAMutex == NULL) {
		cerr << "Error: Failed to create or open clipboardapp mutex." << endl;
		Sleep(3000);
		return EXIT_FAILURE;
	}
	// If mutex is already acquired that means an instance of
	// ClipboardApp.exe is already running.
	DWORD dwTryAcquireMutex = WaitForSingleObject(hCBAMutex, 0);
	if (dwTryAcquireMutex != WAIT_OBJECT_0) {
		cerr << "Note: An instance of ClipboardApp is already running." << endl;
		Sleep(5000);
		return 0;
	}



	// Get the handle to memory mapped file in order to keep it alive and in memory.
	// Openning file mapping
	HANDLE hMapFile = OpenFileMapping(
		FILE_MAP_READ,         // read access
		FALSE,                 // do not inherit the name
		mmfName);              // name of mapping object
	if (hMapFile == NULL) {
		cerr << "Error: Couldn't open file mapping." << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}
	// Aquired MMF handle



	HANDLE hAquiredMMFHandleEvent = OpenEvent(
		SYNCHRONIZE | EVENT_MODIFY_STATE, // Access rights
		FALSE, // No inheritance
		EVENTNAME_AQUIREMMFHANDLE); // Event name
	if (hAquiredMMFHandleEvent == NULL)
	{
		cerr << "Failed to open MMF handle aqusition event" << endl << "Error code = " << GetLastError() << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}

	// Signal the event which will enable CopyApp to finish.
	if (SetEvent(hAquiredMMFHandleEvent) == FALSE)
	{
		cerr << "Failed to signal MMF handle aquistion event object" << endl 
			<< "Error code = " << GetLastError() << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}



	Sleep(MAXRUNNINGTIME);

	CloseHandle(hMapFile);
	ReleaseMutex(hCBAMutex);
	CloseHandle(hCBAMutex);
	CloseHandle(hAquiredMMFHandleEvent);
	return 0;
}