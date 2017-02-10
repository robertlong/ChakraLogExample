#include "Log.h"

Log::Log(const wchar_t* type)
{
	_type = type;
}

void Log::print(const wchar_t* message) {
	wprintf(L"%s: ", _type);
	wprintf(L"%s", message);
	wprintf(L"\n");
}