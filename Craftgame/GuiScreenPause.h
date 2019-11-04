#pragma once
#include "GuiScreen.h"

class GuiScreenPause: public GuiScreen
{
public:
	GuiScreenPause();
	virtual void onLayoutChange(int x, int y, int _width, int _height) override;
	virtual void render(int flags, int mouseX, int mouseY) override;
	virtual void close() override;
};
