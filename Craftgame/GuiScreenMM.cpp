#ifndef SERVER
#include "GuiScreenMM.h"
#include "GameEngine.h"
#include "AnimationFade.h"

GuiScreenMM::GuiScreenMM(std::string title):
	mTitle(title)
{
	animations.push_back(new AnimationFade(this));
}

void GuiScreenMM::onLayoutChange(int x, int y, int _width, int _height)
{
	GuiScreen::onLayoutChange(x, 60, _width, _height - 60);
}
void GuiScreenMM::render(int flags, int mouseX, int mouseY)
{
	GuiScreen::render(flags, mouseX, mouseY);
	glm::vec4 color = getColor();
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(glm::vec4(0, 0, 0, 0.4f));
	glm::mat4 m2 = CGE::instance->renderer->transform;
	CGE::instance->renderer->transform = glm::mat4(1.0);
	CGE::instance->renderer->setMaskingMode(0);
	CGE::instance->renderer->drawRect(0, 0, CGE::instance->width, CGE::instance->height);
	CGE::instance->renderer->setTexturingMode(3);
	CGE::instance->guiShader->loadVector("colors[0]", glm::vec4(0.f, 0.f, 0.f, 0.6f) * color);
	CGE::instance->guiShader->loadVector("colors[1]", glm::vec4(0.f, 0.f, 0.f, 0.6f) * color);
	CGE::instance->guiShader->loadVector("colors[2]", glm::vec4(0.f));
	CGE::instance->guiShader->loadVector("colors[3]", glm::vec4(0.f));
	CGE::instance->renderer->drawRect(0, 0, CGE::instance->width, 60);
	static CFont* f = CGE::instance->fontRegistry->getCFont(_R("craftgame:futuralight"));
	CGE::instance->renderer->drawString(10, 10, mTitle, Align::LEFT, TextStyle::SIMPLE, glm::vec4(1.f), 35, f);
	CGE::instance->renderer->setMaskingMode(1);
	CGE::instance->renderer->transform = m2;
}
#endif