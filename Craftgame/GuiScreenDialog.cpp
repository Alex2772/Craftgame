#ifndef SERVER
#include "GuiScreenDialog.h"
#include "GameEngine.h"
#include "AnimationDialog.h"
#include "Utils.h"
#include "colors.h"

/**
 * \brief Добавляет кнопку в нижнюю часть диалога. Компоновка производится автоматически. По нажатию Enter эмулируется нажатие на первую кнопку
 * \param b Кнопка
 */
void GuiScreenDialog::addButton(GuiButton* b)
{
	if (buttons.empty())
	{
		b->background = this;
	}
	buttons.push_back(b);
	addWidget(b);
}

GuiScreenDialog::GuiScreenDialog(Animation* _a):
	animation(_a)
{
	if (!animation)
		animation = new AnimationDialog(this);
	animations.push_back(animation);
}
void GuiScreenDialog::render(int rf, int mouseX, int mouseY) {
	GuiScreen::render(rf, mouseX, mouseY);
	glm::vec4 color = getColor();
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(glm::vec4(0, 0, 0, 0.4f));
	glm::mat4 m2 = CGE::instance->renderer->transform;
	CGE::instance->renderer->transform = glm::mat4(1.0);
	CGE::instance->renderer->setMaskingMode(0);
	CGE::instance->renderer->drawRect(0, 0, CGE::instance->width, CGE::instance->height);
	CGE::instance->renderer->setMaskingMode(1);
	CGE::instance->renderer->transform = m2;
	CGE::instance->renderer->blur(0, 0, width, height);
	CGE::instance->renderer->setColor(Utils::fromHex(COLOR_BLUE_DARK) * glm::vec4(1, 1, 1, 0.4f));
	CGE::instance->renderer->drawRect(0, 0, width, 80);
	CGE::instance->renderer->drawRect(0, 80, width, height - 80);
	CGE::instance->renderer->setColor(Utils::fromHex(COLOR_BLUE_DARK) * glm::vec4(0, 0, 0, 0.3f));
	CGE::instance->renderer->drawRect(0, 0, width, 4);
	CGE::instance->renderer->drawShadow(0, 0, width, height, glm::vec4(0, 0, 0, 0.5f), 2, 9);
	calculateMask();
	CGE::instance->renderer->setTexturingMode(1);
	CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 1));

	if (!buttons.empty())
	{
		CGE::instance->renderer->setColor(glm::vec4(0, 0, 0, 0.2) * color);
		CGE::instance->renderer->setTexturingMode(0);
		CGE::instance->renderer->drawRect(0, height - 50, width, 50);
	}

	if (!title.empty()) {
		CGE::instance->renderer->drawString(width / 2, 16, title, Align::CENTER, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.6) * color, 22);
	}
}
void GuiScreenDialog::onLayoutChange(int _x, int _y, int _width, int _height) {
	GuiScreen::onLayoutChange(_x, _y, _width, _height);

	unsigned int offset = 5;

	for (size_t i = 0; i < buttons.size(); i++)
	{
		int w = buttons[i]->getMinimumWidth();
		offset += static_cast<unsigned int>(w) + 5;
		buttons[i]->onLayoutChange(width - offset, height - 45, w, 40);
	}
}
void GuiScreenDialog::close() {
	if (animation)
		GuiScreen::animatedClose(animation);
	else
		GuiScreen::close();
}

void GuiScreenDialog::draw(Gui* widget, int flags)
{
	float a = (cos(highlightAnimation / M_PI * 20) + 1) / 2;
	glm::vec4 c = glm::mix(Utils::fromHex(COLOR_BLUE_DARK), Utils::fromHex(COLOR_BLUE_DARK) * 0.8f, a);
	c.a = 1.f;
	CGE::instance->renderer->setMaskingMode(0);
	CGE::instance->renderer->drawShadow(0, 0, widget->width, widget->height, glm::vec4(c.x * 3, c.y * 3, c.z * 3, 1), 2, 7);
	widget->calculateMask();
	CGE::instance->renderer->setMaskingMode(1);

	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(c);
	CGE::instance->renderer->drawRect(0, 0, widget->width, widget->height);

	highlightAnimation += CGE::instance->millis * 0.4f;
	highlightAnimation = fmod(highlightAnimation, 1);
}

void GuiScreenDialog::onKeyDown(Keyboard::Key key, byte data) {
	GuiScreen::onKeyDown(key, data);
	switch (key) {
	case Keyboard::Return:
		if (!buttons.empty())
			buttons[0]->onMouseClick(0, 0, 0);
		break;
	}
}
#endif
