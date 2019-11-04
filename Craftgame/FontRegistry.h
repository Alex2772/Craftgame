#pragma once

#include "CFont.h"
#include <vector>

class CFontRegistry
{
private:
	std::vector<CFont*>* fonts;
public:
	CFontRegistry();
	~CFontRegistry();
	void registerCFont(CFont* font);
	CFont* getCFont(_R res);
};
