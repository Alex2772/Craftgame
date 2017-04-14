#pragma once
#include <string>

class File {
public:
	File(std::string path);
	bool isDir();
	virtual bool isFile();
	virtual void remove();
	std::string path;
};