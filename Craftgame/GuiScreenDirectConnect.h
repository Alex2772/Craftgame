#pragma once
#include "GuiScreenDialog.h"

class GuiScreenDirectConnect : public GuiScreenDialog {
public:
	GuiScreenDirectConnect();
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void onLayoutChange(int _x, int _y, int _width, int _height);
	virtual void onKeyDown(Keyboard::Key key, byte data);

};