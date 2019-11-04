#include "Utils.h"
#ifndef SERVER
#include "GuiTooltip.h"
#include <glm/glm.hpp>
#include "GameEngine.h"
#include "AnimationFade.h"
#include "colors.h"

void GuiTooltip::recalculate()
{
	static CFont* f = CGE::instance->fontRegistry->getCFont(_R("craftgame:default"));
#ifdef WINDOWS
	width = max(int(f->length(title, 18)), min(int(f->length(text, 14)), CGE::instance->width / 3));
#else
	width = glm::max(int(f->length(title, 18)), glm::min(int(f->length(text, 14)), CGE::instance->width / 3));
#endif
	d = f->trimStringToMultiline(text, width);
	width += 8;
	height = 22;
	if (!title.empty()) {
		height += 26;
	}
	height += 16 * d.size();
	visibility = GONE;
	width += 8;
}

GuiTooltip::GuiTooltip()
{
}

void GuiTooltip::setText(std::string t)
{
	text = t;
	if (!text.empty())
		recalculate();
}

void GuiTooltip::setTitle(std::string t)
{
	title = t;
	recalculate();
}

GuiTooltip::GuiTooltip(std::string t):
	text(t)
{
	recalculate();
}

GuiTooltip::GuiTooltip(std::string ti, std::string te):
	title(ti),
	text(te)
{
	recalculate();
}

void GuiTooltip::onLayoutChange(int x, int y, int width, int height)
{
	recalculate();
}

void GuiTooltip::render(int rf, int mouseX, int mouseY)
{
	Gui::render(rf, mouseX, mouseY);
	if (color.a <= 0)
		return;

	CGE::instance->renderer->drawShadow(0, 2, width, height, glm::vec4(0, 0, 0, 1.f) , 4, 9);
	calculateMask();
	/*
	CGE::instance->maskFB->bind(CGE::instance->width, CGE::instance->height);
	CGE::instance->guiShader = CGE::instance->maskShader;
	CGE::instance->guiShader->start();
	CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 1));
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->guiShader = CGE::instance->standartGuiShader;
	CGE::instance->guiShader->start();
	CGE::instance->maskFB->end();*/

	CGE::instance->renderer->blur(0, 0, width, height);
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(Utils::fromHex(COLOR_BLUE_DARK) * glm::vec4(1, 1, 1, 0.6));
	CGE::instance->renderer->drawRect(0, 0, width, height);
	int h = 6;
	if (!title.empty()) {
		CGE::instance->renderer->drawString(8, h, title, LEFT, SIMPLE, glm::vec4(1, 1, 1, 0.9) , 18);
		h += 26;
	}
	for (size_t i = 0; i < d.size(); i++) {
		CGE::instance->renderer->drawString(8, h, d[i], LEFT, SIMPLE, glm::vec4(1, 1, 1, 0.6) );
		h += 16;
	}
}

bool GuiTooltip::empty()
{
	return title.empty() && text.empty();
}
#endif
