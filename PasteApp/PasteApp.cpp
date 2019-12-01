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
		return EXIT_FAILURE;
	}


	// Get the handle to memory mapped file.
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
	LPWSTR pBuf = (LPWSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_READ, // read/write permission
		0,
		0,
		MAXCONTENTSSIZE);

	if (pBuf == NULL)
	{
		cerr << "Error: Couldn't open view for reading." << endl;
		return EXIT_FAILURE;
	}

	wcout << L"Getting contents of memory mapped file..." << endl;
	wcout << pBuf;
	wstringstream contents;
	contents << pBuf;
	
	wcout << "Parsing contents of memory mapped file..." << endl;
	wstring rootDir;
	getline(contents, rootDir, L'\n');
	rootDir = rootDir;

	wstring selectedItem, selectedFiles = L"";
	vector<wstring> selectedDirs;
	while(getline(contents, selectedItem, L'\n')) {
		wstring selectedItemPath = rootDir + L"\\" + selectedItem;
		DWORD dwFileAttributes = GetFileAttributes((LPCWSTR) selectedItemPath.c_str());
		
		if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			selectedDirs.push_back(selectedItem);
		}
		else {
			selectedFiles += qe(selectedItem) + L" ";
		}
	}




	wstring dest = L"F:\\Test";
	
	if (!selectedFiles.empty()) {

		// Copy selected files.
		wcout << L"Copying selected files...";
		wstring comm = L"robocopy.exe " + qe(rootDir) + L" " + qe(dest) + L" " + selectedFiles + L"\n";
		_wsystem(comm.c_str());
		//_wspawnlp(_P_WAIT, L"robocopy.exe", L"robocopy.exe", rootDir.c_str(), dest.c_str(), selectedFiles.c_str());
	}

	// Copy selected directories.
	wcout << L"Copying selected directories...";
	for (int i = 0; i < selectedDirs.size(); i++) {
		wstring sourceDir = qe(rootDir + L"\\" + selectedDirs[i]);
		wstring targetDir = qe(dest + L"\\" + selectedDirs[i]);
		wstring command = L"robocopy " + sourceDir + L" " + targetDir + L" /e \n";

		wcout << L"COMMAND = " + command << endl;
		_wsystem(command.c_str());
	}



	wcout << "FINISHED";
	_wsystem(L"pause");
	return 0;
}