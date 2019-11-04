#ifndef SERVER
#include "GuiLabel.h"
#include "GameEngine.h"

GuiLabel::GuiLabel(std::string s):
	text(s)
{
	font = CGE::instance->fontRegistry->getCFont(_R("craftgame:default"));
}

void GuiLabel::render(int flags, int mouseX, int mouseY)
{
	Gui::render(flags, mouseX, mouseY);
	CGE::instance->renderer->drawString(1, (height - textsize) / 2, text, Align::LEFT, SIMPLE, glm::vec4(1.f), textsize, font);
}

void GuiLabel::setCFont(CFont * f)
{
	font = f;
}

void GuiLabel::setText(std::string t)
{
	text = t;
}

void GuiLabel::setTextSize(long size)
{
	textsize = size;
}
bool GuiLabel::isFocusable() {
	return false;
}
#endif