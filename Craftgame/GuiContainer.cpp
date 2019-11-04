#include <glm/detail/_vectorize.hpp>
#include "def.h"
#ifndef SERVER
#include "GuiContainer.h"

#include "gl.h"
#include "GameEngine.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Joystick.h"

GuiContainer::GuiContainer() {

}

GuiContainer::~GuiContainer() {
	widgets.clear();
}
void GuiContainer::render(int flags, int mouseX, int mouseY) {
	Gui::render(flags, mouseX, mouseY);
	CGE::instance->maskFB->begin();
	CGE::instance->guiShader = CGE::instance->maskShader;
	CGE::instance->guiShader->start();
	CGE::instance->renderer->setColor(glm::vec4(1.f));
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->guiShader = CGE::instance->standartGuiShader;
	CGE::instance->guiShader->start();
	CGE::instance->maskFB->end();
	CGE::instance->postRender([&]() {
		if (CGE::instance->getFocusedGui() && CGE::instance->getFocusedGui()->parent == this) {
			static int _fr = 0;
			static int _s = 0;
			if (!Joystick::getKeyState(Joystick::UP) && !Joystick::getKeyState(Joystick::DOWN) && !Joystick::getKeyState(Joystick::LEFT) && !Joystick::getKeyState(Joystick::RIGHT)) {
				_s = 0;
				_fr = 0;
			}
			else if (_s == 0 || _fr == 0) {
				if (Joystick::_jb[Joystick::UP].strength == 0 && Joystick::getKeyState(Joystick::UP)) {
					findFocus(0);
					if (_s == 0) {
						_fr = 1.f / CGE::instance->millis;
					}
					else if (_fr == 0) {
						_fr = 1.f / CGE::instance->millis / 3.f;
					}
					_s = 1;
					goto _ext;
				}
				if (Joystick::_jb[Joystick::DOWN].strength == 0 && Joystick::getKeyState(Joystick::DOWN)) {
					findFocus(1);
					if (_s == 0) {
						_fr = 1.f / CGE::instance->millis;
					}
					else if (_fr == 0) {
						_fr = 1.f / CGE::instance->millis / 3.f;
					}
					_s = 1;
					goto _ext;
				}
				if (Joystick::_jb[Joystick::LEFT].strength == 0 && Joystick::getKeyState(Joystick::LEFT)) {
					findFocus(2);
					if (_s == 0) {
						_fr = 1.f / CGE::instance->millis;
					}
					else if (_fr == 0) {
						_fr = 1.f / CGE::instance->millis / 3.f;
					}
					_s = 1;
					goto _ext;
				}
				if (Joystick::_jb[Joystick::RIGHT].strength == 0 && Joystick::getKeyState(Joystick::RIGHT)) {
					findFocus(3);
					if (_s == 0) {
						_fr = 1.f / CGE::instance->millis;
					}
					else if (_fr == 0) {
						_fr = 1.f / CGE::instance->millis / 3.f;
					}
					_s = 1;
				}
			}
		_ext:
			if (_fr) {
				_fr--;
			}
		}
	});
}

void GuiContainer::drawWidget(Gui* w, glm::mat4& tr, glm::vec4& c, int _rf, int mouseX, int mouseY)
{
	if (w->visibility == Visibility::VISIBLE) {
		int rf = _rf;
		glm::vec4 v = w->getTransform() * glm::vec4(1, 1, 1, 1);

		if (w->x >= 0 && w->x <= width || w->x + w->width >= 0 && w->x + w->width <= width)
		{
			if (w->y >= 0 && w->y <= height || w->y + w->height >= 0 && w->y + w->height <= height)
			{
				int x = int((v.x - 1) / 2.f * CGE::instance->width);
				int y = int((v.y - 1) / -2.f * CGE::instance->height);
				mouseX = CGE::instance->mouseX;
				mouseY = CGE::instance->mouseY;
				if (!(rf & RenderFlags::OVERLAYED) && x <= mouseX && x + w->width >= mouseX && y <= mouseY && y + w->height >= mouseY)
					rf |= RenderFlags::MOUSE_HOVER;
				int rx = mouseX - x;
				int ry = mouseY - y;
				w->render(rf, rx, ry);
				CGE::instance->renderer->transform = tr;
				CGE::instance->renderer->color = c;
				calculateMask();
			}
		}
	}
}

void pointOf(Gui* v, glm::vec2& dst, const int s) {
	switch (s) {
	case 0:
		dst.x = v->x + v->width / 2;
		dst.y = v->y;
		break;
	case 1:
		dst.x = v->x + v->width / 2;
		dst.y = v->y + v->height;
		break;
	case 2:
		dst.x = v->x;
		dst.y = v->y + v->height / 2;
		break;
	case 3:
		dst.x = v->x + v->width;
		dst.y = v->y + v->height / 2;
		break;
	}
}

void GuiContainer::findFocus(int side) {
	const int opposite[] = {
		1,
		0,
		3,
		2
	};

	if (!focus) {
		for (size_t i = 0; i < widgets.size(); i++)
			if (widgets[i]->isFocusable()) {
				focus = widgets[i].get();
				return;
			}
		if (!widgets.empty())
			focus = widgets.front().get();
		return;
	}
	if (widgets.empty())
		return;
	glm::vec2 point;
	pointOf(focus, point, side);
	bool foundSomething = false;
	Gui* target = widgets.front().get();
	glm::vec2 tarPoint;
	pointOf(target, tarPoint, opposite[side]);
	for (size_t i = 0; i < widgets.size(); i++) {
		Gui* f = widgets[i].get();
		if (!f->isFocusable())
			continue;
		glm::vec2 c;
		pointOf(f, c, opposite[side]);

		if (!foundSomething) {
			switch (side) {
			case 0:
				if (c.y < point.y) {
					foundSomething = true;
				}
				break;
			case 1:
				if (c.y > point.y) {
					foundSomething = true;
				}
				break;
			case 2:
				if (c.x < point.x) {
					foundSomething = true;
				}
				break;
			case 3:
				if (c.x > point.x) {
					foundSomething = true;
				}
				break;
			}
		}

		if (focus != f && glm::length(point - tarPoint) > glm::length(point - c)) {
			target = f;
			tarPoint = c;
		}
	}
	if (!foundSomething && parent) {
		parent->findFocus(side);
		return;
	}
	focus = target;
	if (GuiContainer* c = dynamic_cast<GuiContainer*>(target)) {
		if (!c->focus) {
			c->findFocus(0);
		}
	}
}

void GuiContainer::renderWidgets(int _rf, int mouseX, int mouseY) {
	_rf ^= _rf & RenderFlags::MOUSE_HOVER;
	glm::mat4 tr = getTransform();
	glm::vec4 c = getColor();
	for (int i = 0; i < widgets.size(); i++) {
		drawWidget(widgets[i].get(), tr, c, _rf, mouseX, mouseY);
	}
}

void GuiContainer::onMouseClick(int mX, int mY, int button) {
	for (int i = widgets.size() - 1; i >= 0; i--) {
		if (widgets[i]->visibility != Visibility::GONE) {
			glm::vec4 v = widgets[i]->getTransform() * glm::vec4(1, 1, 1, 1);

			int x = int((v.x - 1) / 2.f * CGE::instance->width);
			int y = int((v.y - 1) / -2.f * CGE::instance->height);
			mX = CGE::instance->mouseX;
			mY = CGE::instance->mouseY;
			if (x <= mX && x + widgets[i]->width >= mX && y <= mY && y + widgets[i]->height >= mY) {
				focus = widgets[i].get();
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

void GuiContainer::addWidget(Gui* gui)
{
	addWidget(std::shared_ptr<Gui>(gui));
}

void GuiContainer::addWidget(std::shared_ptr<Gui> gui) {
	widgets.push_back(gui);
	gui->parent = this;
}
void GuiContainer::onWheel(int mX, int mY, short d) {
	for (int i = widgets.size() - 1; i >= 0; i--) {
		if (widgets[i]->visibility != Visibility::GONE) {
			glm::vec4 v = widgets[i]->getTransform() * glm::vec4(1, 1, 1, 1);

			int x = int((v.x - 1) / 2.f * CGE::instance->width);
			int y = int((v.y - 1) / -2.f * CGE::instance->height);
			mX = CGE::instance->mouseX;
			mY = CGE::instance->mouseY;
			if (x <= mX && x + widgets[i]->width >= mX && y <= mY && y + widgets[i]->height >= mY) {
				focus = widgets[i].get();
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