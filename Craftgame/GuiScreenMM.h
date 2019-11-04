#pragma once
#ifndef SERVER
#include "GuiScreen.h"

class GuiScreenMM: public GuiScreen
{
private:
	std::string mTitle;
public:
	GuiScreenMM(std::string title);

	virtual void onLayoutChange(int x, int y, int _width, int _height) override;
	virtual void render(int flags, int mouseX, int mouseY) override;
};

#endif