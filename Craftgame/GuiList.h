#pragma once

#include "GuiContainer.h"
#include <vector>

class GuiList : public GuiContainer {
private:
	int scroll = 0;
	float motY = 0;
public:
	GuiList();
	virtual void addWidget(Gui* w);
	virtual void render(int flags, int mouseX, int mouseY);
	virtual void onLayoutChange(int x, int y, int width, int height);
	virtual void onWheel(int mouseX, int mouseY, short d);
	virtual void onMouseClick(int mouseX, int mouseY, int button);
};