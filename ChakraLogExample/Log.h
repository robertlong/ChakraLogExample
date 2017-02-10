#pragma once
#include <iostream>

using namespace std;
class Log
{
public:
	void print(const wchar_t* message);
	Log(const wchar_t* type);
private:
	const wchar_t* _type;
};