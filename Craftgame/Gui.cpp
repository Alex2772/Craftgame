#ifndef SERVER
#include "Gui.h"
#include "GuiContainer.h"
#include "GameEngine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
}

void Gui::onMouseClick(int mouseX, int mouseY, int button) {
	if (button == 0 && clickListener)
		clickListener(mouseX, mouseY, button);
}
void Gui::onLayoutChange(int _x, int _y, int _width, int _height) {
	x = _x;
	y = _y;
	width = _width;
	height = _height;
}
bool Gui::isFocused() {
	if (parent != nullptr && parent->focus != nullptr)
		return parent->focus == this;
	return false;
}
void Gui::render(int flags, int mouseX, int mouseY) {
	animTransform = glm::mat4(1.0);
	animColor = glm::vec4(1.0);
	for (std::deque<Animation*>::iterator i = animations.begin(); i != animations.end(); i++)
		(*i)->process();
	CGE::instance->renderer->transform = getTransform();
	CGE::instance->renderer->color = getColor();
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
#endif