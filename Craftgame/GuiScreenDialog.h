#pragma once

#include "GuiScreen.h"

class GuiScreenDialog : public GuiScreen {
private:
	Animation* animation;
public:
	GuiScreenDialog(Animation* animation);
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void onLayoutChange(int x, int y, int width, int height);
	virtual void close();
};