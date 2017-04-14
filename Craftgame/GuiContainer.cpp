#ifndef SERVER
#include "GuiContainer.h"
#include <GL/glew.h>
#include <GL\GL.h>
#include "GameEngine.h"
#include <glm/gtc/matrix_transform.hpp>

GuiContainer::GuiContainer() {

}

GuiContainer::~GuiContainer() {
	for (int i = 0; i < widgets.size(); i++) {
		delete widgets[i];
	}
	widgets.clear();
}
void GuiContainer::render(int flags, int mouseX, int mouseY) {
	Gui::render(flags, mouseX, mouseY);
	CGE::instance->maskFB->begin();
	CGE::instance->guiShader = CGE::instance->maskShader;
	CGE::instance->guiShader->start();
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->guiShader = CGE::instance->standartGuiShader;
	CGE::instance->guiShader->start();
	CGE::instance->maskFB->end();
}
void GuiContainer::renderWidgets(int _rf, int mouseX, int mouseY) {
	_rf ^= _rf & RenderFlags::MOUSE_HOVER;
	glm::mat4 tr = getTransform();
	glm::vec4 c = getColor();
	for (int i = 0; i < widgets.size(); i++) {
		if (widgets[i]->visibility == Visibility::VISIBLE) {
			int rf = _rf;
			glm::vec4 v = widgets[i]->getTransform() * glm::vec4(1, 1, 1, 1);

			int x = int((v.x - 1) / 2.f * CGE::instance->width);
			int y = int((v.y - 1) / -2.f * CGE::instance->height);
			if (!(rf & RenderFlags::OVERLAYED) && x <= mouseX && x + widgets[i]->width >= mouseX && y <= mouseY && y + widgets[i]->height >= mouseY)
				rf |= RenderFlags::MOUSE_HOVER;
			int rx = mouseX - x;
			int ry = mouseY - y;
			widgets[i]->render(rf, rx, ry);
			CGE::instance->renderer->transform = tr;
			CGE::instance->renderer->color = c;
			CGE::instance->maskFB->begin();
			CGE::instance->guiShader = CGE::instance->maskShader;
			CGE::instance->guiShader->start();
			CGE::instance->renderer->drawRect(0, 0, width, height);
			CGE::instance->guiShader = CGE::instance->standartGuiShader;
			CGE::instance->guiShader->start();
			CGE::instance->maskFB->end();
		}
	}
}

void GuiContainer::onMouseClick(int mX, int mY, int button) {
	for (int i = widgets.size() - 1; i >= 0; i--) {
		if (widgets[i]->visibility != Visibility::GONE) {
			glm::vec4 v = widgets[i]->getTransform() * glm::vec4(1, 1, 1, 1);

			int x = int((v.x - 1) / 2.f * CGE::instance->width);
			int y = int((v.y - 1) / -2.f * CGE::instance->height);
			if (x <= mX && x + widgets[i]->width >= mX && y <= mY && y + widgets[i]->height >= mY) {
				focus = widgets[i];
				focus->onMouseClick(mX - x, mY - y, button);
				return;
			}
		}
	}
}

void GuiContainer::onKeyDown(Keyboard::Key key, byte data) {
	if (focus)
		focus->onKeyDown(key, data);
}
void GuiContainer::onLayoutChange(int x, int y, int width, int height) {
	Gui::onLayoutChange(x, y, width, height);
}
void GuiContainer::addWidget(Gui* gui) {
	widgets.push_back(gui);
	gui->parent = this;
}
void GuiContainer::onWheel(int mX, int mY, short d) {
	for (int i = widgets.size() - 1; i >= 0; i--) {
		if (widgets[i]->visibility != Visibility::GONE) {
			glm::vec4 v = widgets[i]->getTransform() * glm::vec4(1, 1, 1, 1);

			int x = int((v.x - 1) / 2.f * CGE::instance->width);
			int y = int((v.y - 1) / -2.f * CGE::instance->height);
			if (x <= mX && x + widgets[i]->width >= mX && y <= mY && y + widgets[i]->height >= mY) {
				focus = widgets[i];
				focus->onWheel(mX, mY, d);
				return;
			}
		}
	}
}
void GuiContainer::onText(char c) {
	if (focus)
		focus->onText(c);
}
#endif