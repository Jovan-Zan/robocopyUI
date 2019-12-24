#include <windows.h>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include <process.h>
#include <io.h>
#include <fcntl.h>
#include <wchar.h>
#include <string>
#include <vector>
#include "..\Shared\constants.h"
using namespace std;

/// <summary>
/// Quote encloses a string with double quotes.
/// </summary>
/// <param name="str">String to enclose with double qoutes.</param>
/// <returns>Qoute enclosed string.</returns>
wstring qe(wstring str) {
	return L"\"" + str + L"\"";
}


int wmain(int argc, wchar_t** argv) {
	if (_setmode(_fileno(stdout), _O_U16TEXT) == -1) {
		cerr << "Error: Can't change stdout mode to unicode." << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}

	if (argc != 2) {
		cerr << "Bad number of arguments (should be 2)" << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}



	// Obtain target folder from argv[1] 
	DWORD dwTargetDirAtt = GetFileAttributes(argv[1]);
	if (!(dwTargetDirAtt & FILE_ATTRIBUTE_DIRECTORY)) {
		cerr << "Passed argument is not a valid directory path!" << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}
	// Else...
	wstring targetDir = wstring(argv[1]);
	// In case argv[1] ends with '"', that is root dir path ended with '\' which escaped '"'
	if (targetDir[targetDir.length() - 1] == '"') 
		targetDir = targetDir.substr(0, targetDir.length() - 1);



	// Get the handle to memory mapped file.
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
	// Creating a view of memory mapped file
	LPWSTR pBuf = (LPWSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_READ, // read/write permission
		0,
		0,
		MAXCONTENTSSIZE);
	if (pBuf == NULL)
	{
		cerr << "Error: Couldn't open view for reading." << endl;
		Sleep(5000);
		return EXIT_FAILURE;
	}



	// Obtain contents of memory mapped file.
	// Before reading from MMF we need to lock the MMF mutex
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
		Sleep(5000);
		return EXIT_FAILURE;
	}
	// Getting contents of memory mapped file
	wstringstream contents;
	contents << pBuf;
	ReleaseMutex(hMMFMutex); // Release MMF mutex
	CloseHandle(hMMFMutex);
	


	// Parsing contents of memory mapped file
	wstring numInFirstLine;
	getline(contents, numInFirstLine, L'\n');
	int itemCount = stoi(numInFirstLine);
	
	wstring rootDir;
	getline(contents, rootDir, L'\n');
	rootDir = rootDir;

	wstring selectedItem, selectedFiles = L"";
	vector<wstring> selectedDirs;
	for (int i = 0; i < itemCount; i++) {
		getline(contents, selectedItem, L'\n');

		wstring selectedItemPath = rootDir + L"\\" + selectedItem;
		DWORD dwFileAttributes = GetFileAttributes((LPCWSTR) selectedItemPath.c_str());
		
		if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			selectedDirs.push_back(selectedItem);
		}
		else {
			selectedFiles += qe(selectedItem) + L" "; // QUOTE ENCLOSING!
		}
	}



	// Copy selected files.
	if (!selectedFiles.empty()) {
		wcout << L"Copying selected files..." << endl;
		wstring comm = L"robocopy.exe " + qe(rootDir) + L" " + qe(targetDir) + L" " + selectedFiles + L"\n";
		_wsystem(comm.c_str());
	}
	// Copy selected directories.
	wcout << L"Copying selected directories...";
	for (int i = 0; i < selectedDirs.size(); i++) {
		wstring sourceSubDir = qe(rootDir + L"\\" + selectedDirs[i]);
		wstring targetSubDir = qe(targetDir + L"\\" + selectedDirs[i]);
		wstring command = L"robocopy " + sourceSubDir + L" " + targetSubDir + L" /e \n";

		wcout << L"COMMAND = " + command << endl;
		_wsystem(command.c_str());
	}



	wcout << L"FINISHED" << endl;
	_wsystem(L"pause");
	return 0;
}