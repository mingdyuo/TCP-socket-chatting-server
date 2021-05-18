#ifndef TO_STRING_H
#define TO_STRING_H

#include <iostream>

template <typename T>
void ToString(T arg) {
	std::cout << arg << std::endl;
}

template <typename T, typename... Types>
void ToString(T arg, Types... args) {
	std::cout << arg << " ";
	ToString(args...);
}


#endif // !TO_STRING_H
