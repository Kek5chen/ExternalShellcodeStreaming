#include "windowhelper.h"

HANDLE GetProcessFromWindow(const wchar_t* window_name)
{
	HWND   hWnd;
	DWORD  procId;
	HANDLE hProc;

	hWnd = FindWindowW(nullptr, window_name);
	if (hWnd == nullptr) {
		throw std::runtime_error("The specified window is not running.");
	}

	GetWindowThreadProcessId(hWnd, &procId);
	if (procId == 0) {
		throw std::runtime_error("Could not GetWindowThreadProcessId on received window handle");
	}

	hProc = OpenProcess(PROCESS_ALL_ACCESS, false, procId);
	if (hProc == nullptr) {
		throw std::runtime_error("Could not retrieve handle from OpenProcess.");
	}

	return hProc;
}
