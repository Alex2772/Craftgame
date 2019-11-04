#ifndef SERVER
#include "GuiText.h"
#include "GameEngine.h"

GuiText::GuiText(std::string t) {
	font = CGE::instance->fontRegistry->getCFont("default");
	setText(t);
}

void GuiText::setText(const std::string& text)
{

	mText = font->trimStringToMultiline(text, width);
}

void GuiText::render(int flags, int mouseX, int mouseY)
{
	Gui::render(flags, mouseX, mouseY);

	for (int i = 0; i < mText.size(); i++)
		CGE::instance->renderer->drawString(0, 14 * 2 * i + 5, mText[i], Align::LEFT, TextStyle::SIMPLE, color);
}
#endif