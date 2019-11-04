#ifndef SERVER
#include "GuiSeekBar.h"
#include "GameEngine.h"
#include "Keyboard.h"
#include "Joystick.h"

GuiSeekBar::GuiSeekBar(std::function<std::string(int)> _onValueChanged, int _maxValue, int _value) :
	maxValue(_maxValue),
	onValueChanged(_onValueChanged),
	value(_value),
	prevValue(_value)

{
	drag = false;
	s = onValueChanged(value);
}
GuiSeekBar::~GuiSeekBar() {

}
void GuiSeekBar::render(int rf, int mouseX, int mouseY) {
	Gui::render(rf, mouseX, mouseY);
	if (!(rf & RenderFlags::NO_BLUR)) {
		CGE::instance->renderer->blur(0, 0, width, height);
	}


	if (CGE::instance->getFocusedGui() == this) {
		Joystick::setJoystickHandler(Joystick::LEFT, Joystick::JoystickButton(1, _("gui.slider"), [&]() {
			if (value) {
				value--;
			}
		}));
		Joystick::setJoystickHandler(Joystick::RIGHT, Joystick::JoystickButton(1, _("gui.slider"), [&]() {
			if (value < maxValue) {
				value++;
			}
		}));
	}

	static size_t c = CGE::instance->guiShader->getUniform("c");
	static size_t texturing = CGE::instance->guiShader->getUniform("texturing");
	int x = Gui::x;
	int y = Gui::y;

	glm::vec4 color = getColor();
	CGE::instance->guiShader->loadInt(texturing, 0);
	CGE::instance->guiShader->loadVector(c, glm::vec4(0.8, 0.8, 0.8, 0.4) * color);
	CGE::instance->renderer->drawRect(0, 0, width, height);
	if (drag) {
		if (!Keyboard::isKeyDown(Keyboard::LButton)) {
			drag = false;
		}
		else {
			float _x = float(mouseX - 5) / float(width - 10);
			value = minValue + glm::clamp(int(round(_x * (maxValue - minValue))), 0, maxValue - minValue);
		}
	}

	int sx = int(float(value - minValue) / float(maxValue - minValue) * (width - 10));


	int i = 1;
	if (enabled && (inside(mouseX, mouseY) || drag) && Keyboard::isKeyDown(Keyboard::LButton) || (mouseX >= sx && mouseX <= sx + 10 && mouseY >= y && mouseY <= y + height))
		i++;
	if (!enabled) {
		i = 0;
	}
	if (i == 0)
		CGE::instance->guiShader->loadVector(c, glm::vec4(0.7, 0.7, 0.7, 0.7) * color);
	else if (i == 1)
		CGE::instance->guiShader->loadVector(c, glm::vec4(1, 1, 1, 0.8) * color);
	else if (i == 2)
		CGE::instance->guiShader->loadVector(c, glm::vec4(1, 1, 1, 1) * color);
	CGE::instance->renderer->drawRect(sx, 0, 10 / 2, height);
	CGE::instance->renderer->drawRect(sx + 10 / 2, 0, 10 / 2, height);

	if (prevValue != value) {
		prevValue = value;
		if (onValueChangingStop)
			onValueChangingStop(value);
		if (onValueChanged)
			s = onValueChanged(value);
	}

	i = 1;
	if (enabled && drag)
		i++;
	if (!enabled) {
		i = 0;
	}
	CGE::instance->guiShader->loadInt(texturing, 1);

	if (i == 0)
		CGE::instance->renderer->drawString(width / 2, height / 2 - 5, s, Align::CENTER, TextStyle::SHADOW, glm::vec4(0.6, 0.6, 0.6, 1) * color);
	else if (i == 1)
		CGE::instance->renderer->drawString(width / 2, height / 2 - 5, s, Align::CENTER, TextStyle::SHADOW, glm::vec4(1, 1, 1, 1) * color);
	else if (i == 2)
		CGE::instance->renderer->drawString(width / 2, height / 2 - 5, s, Align::CENTER, TextStyle::SHADOW, glm::vec4(1, 1, 0.6, 1) * color);

	CGE::instance->guiShader->loadVector(c, glm::vec4(1, 1, 1, 1));
}
void GuiSeekBar::onMouseClick(int x, int y, int button) {
	Gui::onMouseClick(x, y, button);
	if (button == 0 && enabled)
		drag = true;
}

bool GuiSeekBar::isDrag() const
{
	return drag;
}
#endif
