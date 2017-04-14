#ifndef SERVER
#include "GuiList.h"
#include "GameEngine.h"
GuiList::GuiList():
	GuiContainer() {
}
void GuiList::render(int _rf, int mouseX, int mouseY) {
	GuiContainer::render(_rf, mouseX, mouseY);
	_rf ^= _rf & RenderFlags::MOUSE_HOVER;
	if (!(_rf & RenderFlags::NO_BLUR)) {
		CGE::instance->renderer->blur(0, 0, width, height);
	}
	glm::vec4 color = getColor();
	CGE::instance->guiShader->loadInt("texturing", 0.f);
	CGE::instance->guiShader->loadVector("c", glm::vec4(0.1, 0.1, 0.1, 0.7) * color);
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->guiShader->loadVector("c", color);
	int my = scroll;
	for (int i = 0; i < widgets.size(); i++) {
		widgets[i]->y = my;
		int rf = _rf;
		glm::vec4 v = widgets[i]->getTransform() / getTransform() * glm::vec4(1, 1, 1, 1);

		int x = int((v.x - 1) / 2.f * CGE::instance->width);
		int y = int((v.y - 1) / -2.f * CGE::instance->height);
		rf |= RenderFlags::NO_BLUR;
		if (!(rf & RenderFlags::OVERLAYED) && x <= mouseX && x + widgets[i]->width >= mouseX && y <= mouseY && y + widgets[i]->height >= mouseY)
			rf |= RenderFlags::MOUSE_HOVER;
		widgets[i]->render(rf, mouseX - x, mouseY - y);
		my += widgets[i]->height + 1;
	}
	int h = my - scroll;
	if (height < h) {
		if (-scroll + 10 < 0) {
			motY += -scroll * .2f;
		}
		else if (-scroll + height - 20 > h) {
			motY += (-scroll + h - 20) * 0.013f;
		}
		float a = CGE::instance->millis * 20.f;
		motY *= a < 0.5f ? a : 0.5f;
		scroll += motY;
		CGE::instance->renderer->transform = getTransform();
		CGE::instance->guiShader->loadInt("texturing", 0.f);
		float gpn = height * (float(height) / float(h));
		CGE::instance->guiShader->loadVector("c", glm::vec4(0.9, 0.9, 0.9, 0.7) * color);
		CGE::instance->renderer->drawRect(width - 8, float(-scroll) / (h - height) * height - gpn * 0.5, 8, gpn);
		CGE::instance->guiShader->loadInt("texturing", 1.f);
	}
	else {
		scroll = 0;
		motY = 0;
	}
	CGE::instance->guiShader->loadInt("texturing", 1.f);
}
void GuiList::addWidget(Gui* w) {
	GuiContainer::addWidget(w);
	w->x = 0;
}
void GuiList::onMouseClick(int mX, int mY, int button) {
	for (int i = widgets.size() - 1; i >= 0; i--) {
		if (widgets[i]->visibility != Visibility::GONE) {
			glm::vec4 v = widgets[i]->getTransform() / getTransform() * glm::vec4(1, 1, 1, 1);

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
void GuiList::onLayoutChange(int x, int y, int width, int height) {
	Gui::onLayoutChange(x, y, width, height);

	for (int i = 0; i < widgets.size(); i++) {
		widgets[i]->x = 0;
		widgets[i]->width = width;
	}
}
void GuiList::onWheel(int mouseX, int mouseY, short d) {
	motY += d * 1200 * CGE::instance->millis;
}
#endif