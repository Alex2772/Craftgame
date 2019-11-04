#pragma once
#include "Gui.h"
#include <vector>
#include "CFont.h"
#include "FontRegistry.h"

class GuiText: public Gui
{
private:
	std::vector<std::string> mText;
	CFont* font;
public:
	GuiText(std::string text);
	void setText(const std::string& text);
	virtual void render(int flags, int mouseX, int mouseY) override;
};
