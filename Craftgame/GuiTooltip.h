#pragma once
#ifndef SERVER
#include "Gui.h"
#include <vector>

class GuiTooltip: public Gui {
private:
	std::string title;
	std::string text;
	std::vector<std::string> d;
	void recalculate();
public:
	GuiTooltip();
	void setText(std::string t);
	void setTitle(std::string t);
	GuiTooltip(std::string text);
	GuiTooltip(std::string title, std::string text);
	virtual void onLayoutChange(int x, int y, int width, int height);
	virtual void render(int rf, int mouseX, int mouseY);
	bool empty();
};
#endif