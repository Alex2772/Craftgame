#pragma once
#ifndef SERVER
#include "GuiScreen.h"
#include "GuiScreenMM.h"

class GuiScreenSingleplayer : public GuiScreenMM
{
public:
	GuiScreenSingleplayer();

	virtual void close() override;
	virtual void onLayoutChange(int x, int y, int _width, int _height) override;
	virtual void render(int flags, int mouseX, int mouseY) override;
};

#endif