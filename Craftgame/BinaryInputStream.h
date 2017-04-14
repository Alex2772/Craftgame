#pragma once

#include <fstream>
#include "global.h"

using namespace std;

class BinaryInputStream :
	public ifstream {
public:
	BinaryInputStream(std::string file) :
		ifstream(file, std::ios::binary) {

	}
	std::string readString() {
		int size = read<int>();
		char *c = new char[(size_t)size + 1];
		ifstream::read(c, size);
		c[(size_t)size] = '\0';
		return string(c);
	}
	template<class T>
	T read() {
		T t;
		size_t size = sizeof(t);
		ifstream::read((char*)&t, size);
		return t;
	}
};