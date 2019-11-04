#ifndef SERVER
#include "GuiSwitch.h"
#include "Joystick.h"
#include "GameEngine.h"

GuiSwitch::GuiSwitch(std::string _text, std::function<void(bool)> _onValueChange, bool _value) :
	text(_text),
	onValueChange(_onValueChange),
	value(_value) {

}

void GuiSwitch::render(int flags, int mouseX, int mouseY) {
	Gui::render(flags, mouseX, mouseY);

	if (CGE::instance->getFocusedGui() == this) {
		Joystick::setJoystickHandler(Joystick::A, Joystick::JoystickButton(1, _("gui.switch"), [&]() {
			onMouseClick(0, 0, 0);
		}));
	}
	int y = height / 2 - 10;
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(glm::vec4(0.4f, 0.4f, 0.4f, flags & RenderFlags::MOUSE_HOVER ? 0.3f : 0.2f));
	CGE::instance->renderer->drawRect(4, y, 20, 20);
	if (value) {
		CGE::instance->renderer->setColor(glm::vec4(1.f));
		CGE::instance->renderer->drawRect(8, y + 4, 12, 12);
	}
	CGE::instance->renderer->setTexturingMode(1);
	CGE::instance->renderer->drawString(34, y + 4, text, Align::LEFT, TextStyle::SIMPLE, color);
}
void GuiSwitch::onMouseClick(int mouseX, int mouseY, int button) {
	if (!button) {
		value = !value;
		onValueChange(value);
	}
}
#endif