#pragma once
#include <string>
#include <iterator>
#include <iostream>
#include <vector>
#include <memory.h>

enum ArgType {
	T_INTEGER,
	T_STRING,
	T_BOOLEAN,
	T_FLOAT,
	T_FLAG
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
		if (data && dataSize) {
			delete[] data;
		}
	}
	std::string name;
	char* data = nullptr;
	size_t dataSize = 0;
	ArgType type;
};

using namespace std;

namespace Parsing {
	vector<string> splitString(string& s, string delim);
	int getLine(std::istream& istream, string& ss, string delim);
	bool endsWith(string& s, string end);
	bool startsWith(string& s, string start);
	std::vector<CmdLineArg> parseArgsFromString(std::string& s);
};
