#pragma once

#include "GuiButton.h"
#include "GameEngine.h"

class GuiTabButton : public GuiButton {
public:
	GuiTabButton(std::string _text, std::function<void(int, int, int)> _clickListener);
	virtual void render(int flags, int mouseX, int mouseY);
};