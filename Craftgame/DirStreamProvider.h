#pragma once

#include "Res.h"

class DirStreamProvider: public Resource::IStreamProvider
{
private:
	std::string mDir;
public:
	DirStreamProvider(std::string dir);
	virtual std::shared_ptr<std::istream> openStream(std::string) override;
};