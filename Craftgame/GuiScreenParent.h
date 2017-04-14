#pragma once

#ifndef SERVER
#include "GuiScreen.h"

class GuiScreenParent : public GuiScreen {
protected:
	GuiScreen* parent = nullptr;
	bool useBlur = false;
public:
	GuiScreenParent(GuiScreen* parent);
	virtual void render(int rf);
	virtual void renderWidgets(int rf);
	virtual void onLayoutChange(int x, int y, int width, int height);
};

#endif
