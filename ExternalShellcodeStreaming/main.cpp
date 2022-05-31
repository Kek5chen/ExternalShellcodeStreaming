#include "windowhelper.h"

const wchar_t*      wnd_name        = L"ShellCode Target";
constexpr long long p_write_address = <CHECK FOR TARGET OUTPUT>;

static void __stdcall write(int* address)
{
	if (address == nullptr) { return; }
	*address = 5;
}

int get_function_size(void* function)
{
	byte* curByte = reinterpret_cast<byte*>(function);
	int   size    = 1;

	while (*(curByte + size - 1) != 0xC3) {
		size++;
	}
	return size;
}

[[noreturn]] void wait_and_exit_with_error(const char* error)
{
	printf("[X] %s\n", error);
	printf("[X] Exiting in 5 seconds...");
	Sleep(5000);
	ExitProcess(GetLastError());
}

int main()
{
	HANDLE hProc;
	size_t shellcodeSize;
	LPVOID remoteMemory;
	size_t bytesWritten;
	DWORD  threadId;
	HANDLE hThread;
	BOOL   success;

	SetWindowTextW(GetConsoleWindow(), L"Shellcode Injector");

	try {
		hProc = GetProcessFromWindow(wnd_name);
		printf("[+] Found window and received a valid handle\n");
	}
	catch (std::runtime_error& ex) {
		wait_and_exit_with_error("Could not get a valid window handle.");
	}

	shellcodeSize = get_function_size(&write);
	printf("[+] Function write is of %lld bytes in size\n", shellcodeSize);

	remoteMemory = VirtualAllocEx(hProc,
	                              nullptr,
	                              shellcodeSize,
	                              MEM_COMMIT | MEM_RESERVE,
	                              PAGE_EXECUTE_READWRITE);
	if (!remoteMemory) {
		wait_and_exit_with_error("Could not allocate new memory to the remote process.");
	}
	printf("[+] Allocated memory in remote process.\n");

	success = WriteProcessMemory(hProc,
	                             remoteMemory,
	                             &write,
	                             shellcodeSize,
	                             &bytesWritten);
	if (!success || bytesWritten < shellcodeSize) {
		wait_and_exit_with_error("Could not write to remote process.");
	}
	printf("[+] Wrote write into remote process.\n");

	hThread = CreateRemoteThreadEx(hProc,
	                               nullptr,
	                               0,
	                               static_cast<LPTHREAD_START_ROUTINE>(remoteMemory),
	                               reinterpret_cast<LPVOID>(p_write_address),
	                               0,
	                               nullptr,
	                               &threadId);
	if (hThread == INVALID_HANDLE_VALUE) {
		wait_and_exit_with_error("Could not create remote thread.");
	}
	printf("[+] Created thread in remote process.\n");

	printf("[...] Making sure execution finishes before cleanup.\n");
	Sleep(2000);
	printf("[...] Cleaning up\n");

	success = VirtualFreeEx(hProc, remoteMemory, 0, MEM_RELEASE);
	if (!success) {
		wait_and_exit_with_error("Could not release the previously allocated memory page. Crashed?");
	}
	printf("[+] Released the previously allocated memory page.\n");

	CloseHandle(hProc);
	printf("[+] Closed handle. Exiting...\n");
	Sleep(5000);
	ExitProcess(0);
}
