#pragma once

#include "print.hpp"

template<typename ...ArgsType>
void Message(const char* type, const char* pclass, const char* fmt, ArgsType...args) {
	Print("[%][%]:", type, pclass);
	Print(fmt, args...);
	Print("\n");
}

template<typename ...ArgsType>
bool Error(const char* pclass, const char* fmt, ArgsType...args) {
	Message("Error", pclass, fmt, args...);
	return false;
}
