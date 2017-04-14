#ifndef SERVER
#include "GuiButton.h"

#include "GameEngine.h"
#include "GameRenderer.h"

GuiButton::GuiButton(std::string _text, std::function<void(int, int, int)> _clickListener) :
	text(_text) {
	clickListener = _clickListener;
}
void GuiButton::render(int flags, int mouseX, int mouseY) {
	Gui::render(flags, mouseX, mouseY);
	static size_t c = CGE::instance->guiShader->getUniform("c");
	static size_t texturing = CGE::instance->guiShader->getUniform("texturing");
	glm::vec4 color = getColor();
	if (!(flags & RenderFlags::NO_BLUR)) {
		CGE::instance->renderer->blur(0, 0, width, height);
	}
	CGE::instance->guiShader->loadInt(texturing, 0);
	CGE::instance->guiShader->loadVector(c, glm::vec4(0.4f, 0.4f, 0.4f, flags & RenderFlags::MOUSE_HOVER ? 0.3f : 0.1f) * color);
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->guiShader->loadInt(texturing, 1);
	CGE::instance->renderer->drawString(width / 2, 11, text, Align::CENTER, TextStyle::SHADOW, (flags & RenderFlags::MOUSE_HOVER ? glm::vec4(1, 1, 1, 1) : glm::vec4(0.8, 0.8, 0.8, 0.9)) * color);
	CGE::instance->guiShader->loadVector(c, glm::vec4(1, 1, 1, 1));
}
#endif