#pragma once
#include "Gui.h"

class GuiProgressBar: public Gui
{
public:
	float max = 100.f;
	float value = 0.f;
	std::string si = "%";

	GuiProgressBar();

	virtual void render(int flags, int mouseX, int mouseY) override;
};
