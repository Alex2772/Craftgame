#pragma once

#include <string>

class Mod {
public:
	std::string name;
	std::string modid;
	std::string version;
	std::string file;
	Mod();
	virtual ~Mod() = default;
};