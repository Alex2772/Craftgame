#ifndef SERVER
#include "GuiTabButton.h"
#include "Joystick.h"


GuiTabButton::GuiTabButton(std::string _text, std::function<void(int, int, int)> _clickListener) :
	GuiButton(_text, _clickListener) {
}

void GuiTabButton::render(int flags, int mouseX, int mouseY) {
	Gui::render(flags, mouseX, mouseY);

	if (CGE::instance->getFocusedGui() == this) {
		Joystick::setJoystickHandler(Joystick::A, Joystick::JoystickButton(1, _("gui.choose"), [&]() {
			clickListener(0, 0, 0);
		}));
	}
	if (!(flags & RenderFlags::NO_BLUR)) {
		CGE::instance->renderer->blur(0, 0, width, height);
	}
	CGE::instance->renderer->setTexturingMode(0);
	glm::vec4 color = getColor();
	CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, isFocused() ? 0.4f : flags & RenderFlags::MOUSE_HOVER ? 0.3f : 0.1f) * color);
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->renderer->setTexturingMode(1);
	CGE::instance->renderer->drawString(width / 2, 11, text, Align::CENTER, TextStyle::SHADOW, (flags & RenderFlags::MOUSE_HOVER || isFocused() ? glm::vec4(1, 1, 1, 1) : glm::vec4(0.8, 0.8, 0.8, 0.9)) * color);
	CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 1));
}
#endif