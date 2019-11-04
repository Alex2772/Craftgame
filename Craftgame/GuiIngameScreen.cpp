#ifndef SERVER
#include "GuiIngameScreen.h"
#include "GameEngine.h"
#include "Utils.h"

void GuiIngameScreen::render(int flags, int mouseX, int mouseY)
{
	GuiScreen::render(flags, mouseX, mouseY);

	CGE::instance->renderer->drawShadow(0, 4, width, height - 4, glm::vec4(0, 0, 0, 1.f), 3, 11);
	calculateMask();
	
	
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(glm::vec4(0, 0, 0, 0.4f));
	glm::mat4 m2 = CGE::instance->renderer->transform;
	CGE::instance->renderer->transform = glm::mat4(1.0);
	CGE::instance->renderer->setMaskingMode(0);
	CGE::instance->renderer->drawRect(0, 0, CGE::instance->width, CGE::instance->height);
	CGE::instance->renderer->setMaskingMode(1);
	CGE::instance->renderer->transform = m2;
	/*

	static glm::vec4 c_bg = Utils::fromHex(0xc6c6c6ff);
	static glm::vec4 c_light = glm::vec4(1);
	static glm::vec4 c_dark = Utils::fromHex(0x555555ff);
	static glm::vec4 c_bl = glm::vec4(0, 0, 0, 1);

	CGE::instance->renderer->setTexturingMode(0);

	// Background
	CGE::instance->renderer->setColor(c_bg);
	CGE::instance->renderer->drawRect(2, 2, width - 4, height - 4);

	// Border
	CGE::instance->renderer->setColor(c_bl);
	CGE::instance->renderer->drawRect(0, 4, 2, height - 8);
	CGE::instance->renderer->drawRect(4, 0, width - 8, 2);
	CGE::instance->renderer->drawRect(width - 2, 4, 2, height - 8);
	CGE::instance->renderer->drawRect(4, height - 2, width - 8, 2);
	
	// Light
	CGE::instance->renderer->setColor(c_light);
	CGE::instance->renderer->drawRect(4, 2, width - 8, 4);
	CGE::instance->renderer->drawRect(2, 4, 4, height - 8);
	CGE::instance->renderer->drawRect(6, 6, 2, 2);

	// Dark
	CGE::instance->renderer->setColor(c_dark);
	CGE::instance->renderer->drawRect(width - 6, 4, 4, height - 8);
	CGE::instance->renderer->drawRect(4, height - 6, width - 8, 4);
	CGE::instance->renderer->drawRect(width - 8, height - 8, 2, 2);

	// Overlay fix
	CGE::instance->renderer->setColor(c_bg);
	CGE::instance->renderer->drawRect(4, height - 6, 2, 2);
	CGE::instance->renderer->drawRect(width - 6, 4, 2, 2);

	// Border points
	CGE::instance->renderer->setColor(c_bl);
	CGE::instance->renderer->drawRect(2, 2, 2, 2);
	CGE::instance->renderer->drawRect(width - 4, 2, 2, 2);
	CGE::instance->renderer->drawRect(width - 4, height - 4, 2, 2);
	CGE::instance->renderer->drawRect(2, height - 4, 2, 2);

	// Warning
	FontStyle fs;
	fs.color = c_dark;
	CGE::instance->renderer->drawString(width / 2, 10, "! WARNING !", Align::CENTER, fs);
	CGE::instance->renderer->drawString(width / 2, 25, "STOLEN FROM MINECRAFT", Align::CENTER, fs);*/
}
#endif