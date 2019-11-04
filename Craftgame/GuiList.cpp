#ifndef SERVER
#include "GuiList.h"
#include "GameEngine.h"
GuiList::GuiList():
	GuiContainer() {
}
void GuiList::render(int _rf, int mouseX, int mouseY) {
	GuiContainer::render(_rf, mouseX, mouseY);
	/*
	_rf ^= _rf & RenderFlags::MOUSE_HOVER;
	if (!(_rf & RenderFlags::NO_BLUR)) {
		CGE::instance->renderer->blur(0, 0, width, height);
	}
	CGE::instance->renderer->setTexturingMode(0.f);
	CGE::instance->renderer->setColor(glm::vec4(0.1, 0.1, 0.1, 0.7));
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->renderer->setColor(color);*/
	int my = scroll;
	for (int i = 0; i < widgets.size(); i++) {
		widgets[i]->y = my;
		int rf = _rf;
		glm::vec4 v = widgets[i]->getTransform() / getTransform() * glm::vec4(1, 1, 1, 1);


		if (widgets[i]->x >= 0 && widgets[i]->x <= width || widgets[i]->x + widgets[i]->width >= 0 && widgets[i]->x + widgets[i]->width <= width)
		{
			if (widgets[i]->y >= 0 && widgets[i]->y <= height || widgets[i]->y + widgets[i]->height >= 0 && widgets[i]->y + widgets[i]->height <= height)
			{
				int x = int((v.x - 1) / 2.f * CGE::instance->width);
				int y = int((v.y - 1) / -2.f * CGE::instance->height);
				rf |= RenderFlags::NO_BLUR;
				rf &= ~RenderFlags::MOUSE_HOVER;
				if (!(rf & RenderFlags::OVERLAYED) && x <= mouseX && x + widgets[i]->width >= mouseX && y <= mouseY && y + widgets[i]->height >= mouseY)
					rf |= RenderFlags::MOUSE_HOVER;
				widgets[i]->calculateMask();
				widgets[i]->render(rf, mouseX - x, mouseY - y);
				CGE::instance->renderer->color = getColor();
				
			}
			my += widgets[i]->height + 1;
		}
	}
	int h = my - scroll;
	if (height < h) {
		if (-scroll + 10 < 0) {
			motY += -scroll * 20.f;
		}
		else if (-scroll + height - 20 > h) {
			motY += (-scroll - 40);
		}
		float a = 20.f;
		motY *= a < 0.5f ? a : 0.5f;
		scroll += CGE::instance->millis * motY;
		CGE::instance->renderer->transform = getTransform();
		CGE::instance->renderer->setTexturingMode(0.f);
		float gpn = height * (float(height) / float(h));
		CGE::instance->renderer->setColor(glm::vec4(0.9, 0.9, 0.9, 0.7));
		CGE::instance->renderer->drawRect(width - 8, float(-scroll) / (h - height) * height - gpn * 0.5, 8, gpn);
		CGE::instance->renderer->setTexturingMode(1.f);
	}
	else {
		scroll = 0;
		motY = 0;
	}
	CGE::instance->renderer->setTexturingMode(1.f);
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
				focus = widgets[i].get();
				focus->onMouseClick(mX - x, mY - y, button);
				return;
			}
		}
	}
}

void GuiList::clear()
{
	widgets.clear();
}

void GuiList::onLayoutChange(int x, int y, int width, int height) {
	Gui::onLayoutChange(x, y, width, height);

	for (int i = 0; i < widgets.size(); i++) {
		widgets[i]->onLayoutChange(0, 0, width, 40);
	}
}
void GuiList::onWheel(int mouseX, int mouseY, short d) {
	motY += d * 1200;
}
#endif