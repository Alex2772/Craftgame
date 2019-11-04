#pragma once

#include "GuiContainer.h"

class GuiMenu: public GuiContainer
{
public:
	GuiMenu();
	virtual void onLayoutChange(int x, int y, int width, int height);
	virtual void render(int flags, int mouseX, int mouseY);
	void hide();
};