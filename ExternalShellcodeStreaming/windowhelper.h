#pragma once
#include <Windows.h>
#include <stdexcept>

HANDLE GetProcessFromWindow(const wchar_t* window_name);