#pragma once
#ifndef SERVER
#include "Gui.h"
#include "CFont.h"

class GuiLabel : public Gui {
private:
	long textsize = 16;
	std::string text;
	CFont* font;
public:
	GuiLabel(std::string s = "");
	virtual void render(int flags, int mouseX, int mouseY);
	void setCFont(CFont* f);
	void setText(std::string t);
	void setTextSize(long size);
	virtual bool isFocusable();
};
#endif