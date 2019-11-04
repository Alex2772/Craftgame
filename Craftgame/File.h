#pragma once
#include <string>

class File {
public:
	File(std::string path);
	bool isDir() const;
	virtual bool isFile();
	virtual void remove();
	std::string name() const;
	std::string path;
};