#ifndef SERVER
#include "Gui.h"
#include "GuiContainer.h"
#include "GuiTooltip.h"
#include "GuiMenu.h"
#include "GameEngine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TOOLTIP_W 300
#define TOOLTIP_H 200

Gui::Gui():
x(0),
y(0),
width(400),
height(40),
transform(1.0f) {
}

Gui::~Gui() {
	while (animations.size()) {
		delete animations.back();
		animations.pop_back();
	}
	if (tooltip)
		delete tooltip;
	if (contextMenu)
		delete contextMenu;
	if (CGE::instance && CGE::instance->fcs == this)
	{
		CGE::instance->fcs = nullptr;
	}
}
void Gui::onMouseClick(int mouseX, int mouseY, int button) {
	if (button == 0 && clickListener)
		clickListener(mouseX, mouseY, button);

	if (contextMenu) {
		if (contextMenu->visibility != VISIBLE && button == 1)
		{
			CGE::instance->contextMenu = contextMenu;
			contextMenu->visibility = VISIBLE;
			contextMenu->x = CGE::instance->mouseX;
			contextMenu->y = CGE::instance->mouseY;
			contextMenu->onLayoutChange(contextMenu->x, contextMenu->y, 0, 0);
			if (contextMenu->x + contextMenu->width > CGE::instance->width)
			{
				contextMenu->x = CGE::instance->mouseX - 15 - contextMenu->width;
			}
			if (contextMenu->y + contextMenu->height > CGE::instance->height)
			{
				contextMenu->y = CGE::instance->mouseY - 18 - contextMenu->height;
			}
		}
	}
}
void Gui::onLayoutChange(int _x, int _y, int _width, int _height) {
	x = _x;
	y = _y;
	width = _width;
	height = _height;
	if (tooltip)
		tooltip->onLayoutChange(0, 0, 0, 0);
}
bool Gui::isFocused() {
	if (parent && parent->focus)
		return parent->focus == this;
	return false;
}
bool Gui::isFocusable() {
	return visibility != GONE;
}
void Gui::render(int flags, int mouseX, int mouseY) {
	animTransform = glm::mat4(1.0);
	animColor = glm::vec4(1.0);
	for (std::deque<Animation*>::iterator i = animations.begin(); i != animations.end(); i++)
		(*i)->process();

	if (tooltip && (!dynamic_cast<GuiTooltip*>(tooltip) || !dynamic_cast<GuiTooltip*>(tooltip)->empty())) {
		if (flags & RenderFlags::MOUSE_HOVER) {
			CGE::instance->postRender([&, flags]() {
				tooltip->visibility = VISIBLE;
				tooltip->x = CGE::instance->mouseX + 18;
				tooltip->y = CGE::instance->mouseY + 32;
				if (tooltip->x + tooltip->width > CGE::instance->width)
				{
					tooltip->x = CGE::instance->mouseX - 15 - tooltip->width;
				}
				if (tooltip->y + tooltip->height > CGE::instance->height)
				{
					tooltip->y = CGE::instance->mouseY - 18 - tooltip->height;
				}
				tooltip->render(flags ^ RenderFlags::MOUSE_HOVER, CGE::instance->mouseX, CGE::instance->mouseY);
			});
		}
		else {
			tooltip->visibility = GONE;
		}
	}
	if (contextMenu && contextMenu->visibility == VISIBLE) {
		CGE::instance->postRender([&, flags]() {
			contextMenu->render(flags ^ RenderFlags::MOUSE_HOVER, CGE::instance->mouseX, CGE::instance->mouseY);
		});
	}

	CGE::instance->renderer->transform = getTransform();
	CGE::instance->renderer->color = getColor();
}

void Gui::calculateMask()
{
	CGE::instance->renderer->transform = getTransform();
	CGE::instance->renderer->color = getColor();
	CGE::instance->maskFB->begin();
	CGE::instance->guiShader = CGE::instance->maskShader;
	CGE::instance->guiShader->start();
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->guiShader = CGE::instance->standartGuiShader;
	CGE::instance->maskFB->end();
	CGE::instance->guiShader->start();
}

glm::mat4 Gui::getTransform() {
	glm::vec3 vv = glm::vec3(((float)x) / CGE::instance->renderer->viewportW * 2.f, -((float)y) / CGE::instance->renderer->viewportH * 2.f, 0);
	glm::mat4 m = glm::translate(parent == nullptr ? glm::mat4(1.f) : parent->getTransform(), vv);
	return m * transform * animTransform;
}
glm::vec4 Gui::getColor() {
	return (parent == nullptr ? color : parent->getColor() * color) * animColor;
}

void Gui::onWheel(int mouseX, int mouseY, short d) {

}

void Gui::onKeyDown(Keyboard::Key key, byte data) {
}

void Gui::onText(char c) {

}
/**
 * \brief Минимальная ширина UI компонента, при котором он будет выглядеть нормально и максимально компактно
 */
int Gui::getMinimumWidth()
{
	return width;
}
#endif