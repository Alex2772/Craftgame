#ifndef SERVER
#include "GuiSwitch.h"

#include "GameEngine.h"

GuiSwitch::GuiSwitch(std::string _text, std::function<void(bool)> _onValueChange, bool _value) :
	text(_text),
	onValueChange(_onValueChange),
	value(_value) {

}

void GuiSwitch::render(int flags, int mouseX, int mouseY) {
	Gui::render(flags, mouseX, mouseY);
	int y = height / 2 - 10;
	static size_t c = CGE::instance->guiShader->getUniform("c");
	static size_t texturing = CGE::instance->guiShader->getUniform("texturing");
	glm::vec4 color = getColor();
	CGE::instance->guiShader->loadInt(texturing, 0.f);
	CGE::instance->guiShader->loadVector(c, glm::vec4(0.4f, 0.4f, 0.4f, flags & RenderFlags::MOUSE_HOVER ? 0.3f : 0.2f) * color);
	CGE::instance->renderer->drawRect(4, y, 20, 20);
	if (value) {
		CGE::instance->guiShader->loadVector(c, color);
		CGE::instance->renderer->drawRect(8, y + 4, 12, 12);
	}
	CGE::instance->guiShader->loadInt(texturing, 1.f);
	CGE::instance->renderer->drawString(34, y + 4, text, Align::LEFT, TextStyle::SIMPLE, color);
}
void GuiSwitch::onMouseClick(int mouseX, int mouseY, int button) {
	if (!button) {
		value = !value;
		onValueChange(value);
	}
}
#endif