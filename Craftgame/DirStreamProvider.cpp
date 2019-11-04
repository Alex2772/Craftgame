#include "DirStreamProvider.h"
#include <fstream>

DirStreamProvider::DirStreamProvider(std::string dir):
	mDir(dir)
{
	
}

std::shared_ptr<std::istream> DirStreamProvider::openStream(std::string file)
{
	return std::make_shared<std::ifstream>(mDir + "/" + file, std::ios::binary);
}

