#pragma once

#include <fstream>
#include "global.h"



class BinaryInputStream :
	public std::ifstream {
public:
	BinaryInputStream(std::string file) :
		std::ifstream(file, std::ios::binary) {

	}
	std::string readString() {
		int size = read<int>();
		char *c = new char[(size_t)size + 1];
		std::ifstream::read(c, size);
		c[(size_t)size] = '\0';
		return std::string(c);
	}
	template<class T>
	T read() {
		T t;
		size_t size = sizeof(t);
		std::ifstream::read((char*)&t, size);
		return t;
	}
};