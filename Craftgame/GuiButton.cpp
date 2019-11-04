#ifndef SERVER
#include "GuiButton.h"
#include "colors.h"
#include "GameEngine.h"
#include "GameRenderer.h"
#include "Utils.h"
#include "Joystick.h"

GuiButton::GuiButton(std::string _text, std::function<void(int, int, int)> _clickListener) :
	text(_text),
	background(this){
	clickListener = _clickListener;
}
void GuiButton::render(int flags, int mouseX, int mouseY) {
	Gui::render(flags, mouseX, mouseY);

	if (dynamic_cast<GuiMenu*>(parent))
	{
		textAligment = Align::LEFT;
	}

	if (CGE::instance->getFocusedGui() == this) {
		Joystick::setJoystickHandler(Joystick::A, Joystick::JoystickButton(1, _("gui.choose"), [&]() {
			clickListener(0, 0, 0);
		}));
	}

	if (!(flags & RenderFlags::NO_BLUR)) {
		CGE::instance->renderer->blur(0, 0, width, height);
	}
	if (background)
		background->draw(this, flags);
	CGE::instance->renderer->setTexturingMode(1);
	if (textAligment == Align::CENTER)
		CGE::instance->renderer->drawString(width / 2, 11, text, textAligment, TextStyle::SHADOW, (flags & RenderFlags::MOUSE_HOVER ? glm::vec4(1, 1, 1, 1) : glm::vec4(0.8, 0.8, 0.8, 0.9)) * color);
	else
		CGE::instance->renderer->drawString(10, 11, text, textAligment, TextStyle::SHADOW, (flags & RenderFlags::MOUSE_HOVER ? glm::vec4(1, 1, 1, 1) : glm::vec4(0.8, 0.8, 0.8, 0.9)) * color);
	CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 1));
}

void GuiButton::draw(Gui* g, int flags)
{
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(Utils::fromHex(COLOR_BLUE) * glm::vec4(1.f, 1.f, 1.f, flags & RenderFlags::MOUSE_HOVER ? 0.3f : 0.1f) * color);
	CGE::instance->renderer->drawRect(0, 0, width, height);
}

int GuiButton::getMinimumWidth()
{
	return int(CGE::instance->fontRegistry->getCFont(_R("default"))->length(text)) + 20;
}
#endif
