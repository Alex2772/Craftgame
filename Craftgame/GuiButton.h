#pragma once

#include "Gui.h"
#include "IDrawable.h"
#include "GameRenderer.h"

class GuiButton : public Gui, public IDrawable {
public:
	GuiButton(std::string text, std::function<void(int, int, int)> clickListener);
	virtual void render(int flags, int mouseX, int mouseY);
	virtual void draw(Gui* g, int flags);
	virtual int getMinimumWidth();
	std::string text;
	Align textAligment = Align::CENTER;
	IDrawable* background;
};
