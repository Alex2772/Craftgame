#pragma once

#include "Gui.h"

class GuiButton : public Gui {
public:
	GuiButton(std::string text, std::function<void(int, int, int)> clickListener);
	void render(int flags, int mouseX, int mouseY);
	std::string text;
};