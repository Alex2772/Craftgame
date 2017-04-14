#pragma once

#ifdef WINDOWS
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include "File.h"
#include <vector>

class Dir: public File {
public:
	Dir(std::string path);
	bool mkdir();
	std::vector<File> list();
};
