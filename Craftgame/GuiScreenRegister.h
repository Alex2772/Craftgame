#pragma once
#ifndef SERVER
#include "GuiScreenDialog.h"

class GuiScreenRegister: public GuiScreenDialog
{
public:
	GuiScreenRegister(std::function<void(const std::string&)>);
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void onLayoutChange(int _x, int _y, int _width, int _height);
};
#endif