#include <cstdio>
#include "Windows.h" 

[[noreturn]] int main()
{
	SetWindowTextW(GetConsoleWindow(), L"ShellCode Target");
	long long number = 1;
	while(true)
	{
		printf("write to %p: %lld\n", static_cast<void*>(&number), number);
		Sleep(1000);
	}
}
