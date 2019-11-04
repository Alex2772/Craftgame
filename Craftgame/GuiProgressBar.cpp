#ifndef SERVER
#include "Utils.h"
#include "GuiProgressBar.h"
#include "GameEngine.h"

GuiProgressBar::GuiProgressBar()
{
	height = 6 + 14;
}

void GuiProgressBar::render(int flags, int mouseX, int mouseY)
{
	Gui::render(flags, mouseX, mouseY);

	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(glm::vec4(1.f, 1.f, 1.f, 0.2f));
	CGE::instance->renderer->drawRect(0, 0, width, 6);
	CGE::instance->renderer->setColor(Utils::fromHex(0x378deeaa));
	CGE::instance->renderer->drawRect(0, 0, width * (value / max), 6);
	std::string mText = std::to_string(int(max)) + si;
	static CFont* font = CGE::instance->fontRegistry->getCFont("default");
	int len = font->length(mText);
	CGE::instance->renderer->drawString(len = width - len - 4, 7, mText);
	mText = std::to_string(int(value)) + si;
	len -= 4;
	int h = font->length(mText);
	len -= h;
	int w = width * (value / max) - h;
	len = (glm::clamp)(w, 2, len);
	CGE::instance->renderer->drawString(len, 7, mText);
}

#endif