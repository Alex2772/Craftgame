#pragma once
#include "Gui.h"

class GuiSwitch : public Gui {
public:
	GuiSwitch(std::string text, std::function<void(bool)> onValueChange, bool value = false);
	virtual void render(int flags, int mouseX, int mouseY);
	virtual void onMouseClick(int mouseX, int mouseY, int button);
	std::string text;
	std::function<void(bool)> onValueChange;
	bool value = false;
};