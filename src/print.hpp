#ifndef CORTEX_PRINT_HPP
#define CORTEX_PRINT_HPP

#include <iostream>

inline void Print(const char *fmt){
	std::cout << fmt;	
}

template <typename T, typename ...ArgsType>
inline void Print(const char *fmt, const T &arg, const ArgsType&...args) {
	while (*fmt) {
		char ch = *fmt++;
		if (ch == '%') {
			std::cout << arg;
			return Print(fmt, args...);
		}
		std::cout << ch;
	}
}

template <typename ...ArgsType>
inline void Println(const char *fmt, const ArgsType&...args){
	Print(fmt, args...);
	Print("\n");
}

#endif//CORTEX_PRINT_HPP