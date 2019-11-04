#ifndef SERVER
#include "GuiCloseButton.h"
#include "GameEngine.h"
#include "Joystick.h"


GuiCloseButton::GuiCloseButton(GLuint _i, std::function<void(int, int, int)> click):
	i(_i)
{
	width = 48;
	height = 32;
	clickListener = click;
}

void GuiCloseButton::render(int flags, int mouseX, int mouseY)
{
	Gui::render(flags, mouseX, mouseY);
	if (CGE::instance->getFocusedGui() == this) {
		Joystick::setJoystickHandler(Joystick::A, Joystick::JoystickButton(1, _("gui.close"), [&]() {
			clickListener(0, 0, 0);
		}));
	}
	static size_t c = CGE::instance->guiShader->getUniform("c");
	static size_t texturing = CGE::instance->guiShader->getUniform("texturing");

	if (flags & RenderFlags::MOUSE_HOVER) {
		animation += CGE::instance->millis * 5.f;
	}
	else {
		animation -= CGE::instance->millis * 3.4f;
	}
	animation = glm::clamp(animation, 0.f, 1.f);
	
	CGE::instance->guiShader->loadInt(texturing, 0);
	CGE::instance->guiShader->loadVector(c, glm::mix((glm::vec4(0.6, 0, 0, 0)), glm::vec4(0.8, 0, 0, 1), animation));
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->guiShader->loadInt(texturing, 1);
	CGE::instance->renderer->setColor(color);
	glBindTexture(GL_TEXTURE_2D, i);
	CGE::instance->renderer->drawRect(8, 0, 32, 32);
	CGE::instance->guiShader->loadVector(c, glm::vec4(1, 1, 1, 1));
}
#endif
