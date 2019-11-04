#pragma once

#include "CraftgameException.h"

class HttpException : public  CraftgameException {
public:
	HttpException(const char* st, unsigned short s):
		CraftgameException(st),
		status(s)
	{

	}
	unsigned short status;
};
