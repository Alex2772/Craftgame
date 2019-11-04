#pragma once
#include <string>
#include <iterator>
#include <iostream>
#include <vector>
#include <memory.h>
#include "Block.h"

class Command;

enum ArgType {
	T_NUMBER = 1,
	T_INTEGER = 2 | T_NUMBER,
	T_STRING = 4,
	T_BOOLEAN = 8,
	T_FLOAT = 16 | T_NUMBER,
	T_FLAG = 32,
};

class CmdLineArg {
public:
	CmdLineArg() {}
	CmdLineArg(const CmdLineArg& c):
		type(c.type),
		name(c.name),
		dataSize(c.dataSize)
	{
		if (dataSize && c.data) {
			data = new char[dataSize];
			memcpy(data, c.data, dataSize);
		}
	}
	~CmdLineArg() {
		clear();
	}
	template <class T>
	T get()
	{
		return *reinterpret_cast<T*>(data);
	}

	std::string toString();
	float toFloat();
	int toInt();
	void clear();
	std::string name;
	char* data = nullptr;
	size_t dataSize = 0;
	ArgType type;
};



namespace Parsing {
	std::vector<std::string> splitString(const std::string& s, std::string delim);
	int getLine(std::istream& istream, std::string& ss, std::string delim);
	bool endsWith(std::string& s, std::string end);
	bool startsWith(std::string& s, std::string start);
	std::vector<CmdLineArg> parseArgsFromString(std::string& s);
	void parseArgsServer(std::vector<CmdLineArg>& vect, Command*& cmd, std::string& s);
};
