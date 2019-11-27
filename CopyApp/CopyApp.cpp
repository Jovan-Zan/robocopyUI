#include "pch.h"
#include <iostream>
#include <string>
#include <cstdlib>
 
#include <Shobjidl_core.h>
#include <ShlDisp.h>
#include <exdisp.h >
#include <windows.h>
#include <commctrl.h>
#include <fileapi.h>
using namespace std;

int main(int argc, char *argv[])
{
	// Foreground window handle
	HWND fgwh;
	HWND dwh;
	char wname[1024];

	// Returns foreground window handle.
	fgwh = GetForegroundWindow();
	cout << "Foreground window handle: " << fgwh << endl;

	// Returns window name (not path!).
	GetWindowTextA(fgwh, wname, 1024);

	cout << "Window name: " << wname << endl ;
	Sleep(2000);

 	return 0;
}