#pragma once

#include "Font.h"
#include <vector>

class FontRegistry
{
private:
	std::vector<Font*>* fonts;
public:
	FontRegistry();
	~FontRegistry();
	void registerFont(Font* font);
	Font* getFont(_R& res);
};
