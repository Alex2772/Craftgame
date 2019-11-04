#pragma once
#ifndef SERVER
#include "GuiScreen.h"

class GuiIngameScreen: public GuiScreen
{
public:

	virtual void render(int flags, int mouseX, int mouseY) override;
};
#endif