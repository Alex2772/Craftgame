#pragma once
#ifndef SERVER
#include "GuiScreen.h"
#include "GuiScreenMM.h"

class GuiScreenMultiplayer: public GuiScreenMM
{
public:
	GuiScreenMultiplayer();

	virtual void close() override;
	virtual void onLayoutChange(int x, int y, int _width, int _height) override;
	virtual void render(int flags, int mouseX, int mouseY) override;
};

#endif