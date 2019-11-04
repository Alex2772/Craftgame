#ifndef SERVER
#include "GuiMenu.h"
#include "GuiContainer.h"
#include "GameEngine.h"
#include "Utils.h"
#include "colors.h"
GuiMenu::GuiMenu()
{
	visibility = GONE;
}

void GuiMenu::onLayoutChange(int x, int y, int width, int height)
{
	width = 150;
	height = 0;
	for (std::shared_ptr<Gui> w : widgets)
	{
		w->x = 0;
		w->width = width;
		w->y = height;
		height += w->height;
	}
	Gui::onLayoutChange(x, y, width, height);
}

void GuiMenu::render(int flags, int mouseX, int mouseY)
{
	GuiContainer::render(flags, mouseX, mouseY);
	CGE::instance->renderer->drawShadow(0, 2, width, height, glm::vec4(0, 0, 0, 1.f), 4, 9);
	calculateMask();

	CGE::instance->renderer->blur(0, 0, width, height);
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(Utils::fromHex(COLOR_BLUE_DARK) * glm::vec4(1, 1, 1, 0.6));
	CGE::instance->renderer->drawRect(0, 0, width, height);
	GuiContainer::renderWidgets(flags, mouseX, mouseY);
}
void GuiMenu::hide()
{
	CGE::instance->contextMenu = nullptr;
	visibility = GONE;
}
#endif