#ifndef SERVER

#include "GuiWindow.h"
#include "GameEngine.h"
#include "Utils.h"
#include "GuiCloseButton.h"
#include "AnimationDialog.h"
#include "Keyboard.h"

GuiWindow::GuiWindow(std::string t, size_t x, size_t y, size_t width, size_t height):
	title(t)
{
	height += 35;
	addTitleButton(new GuiCloseButton(CGE::instance->textureManager->loadTexture(_R("craftgame:res/gui/close.png")), [&](int, int, int)
                                  {
	                                  close();
                                  }));
	onLayoutChange(x, y, width, height);
	animations.push_back(openClose = new AnimationDialog(this));
}

GuiWindow::GuiWindow(std::string t, size_t width, size_t height) :
	GuiWindow(t, (CGE::instance->width - width) / 2, (CGE::instance->height - height) / 2, width, height)
{
}

void GuiWindow::onMouseClick(int mouseX, int mouseY, int button)
{
	if (button == 0)
	{
		if (mouseY <= 35)
		{
			for (size_t i = 0; i < buttons.size(); i++)
			{
				if (buttons[i]->visibility != Visibility::GONE)
				{
					int rf = 0;
					glm::mat4 f = buttons[i]->getTransform() / getTransform();
					glm::vec4 v = f * glm::vec4(1, 1, 1, 1);

					int x = int((v.x - 1) / 2.f * CGE::instance->width);
					int y = int((v.y - 1) / -2.f * CGE::instance->height);
					if (!(rf & RenderFlags::OVERLAYED) && x <= mouseX && x + buttons[i]->width >= mouseX && y <= mouseY && y + buttons[i]->height >= mouseY)
					{
						int rx = mouseX - x;
						int ry = mouseY - y;
						buttons[i]->onMouseClick(rx, ry, button);
						return;
					}
				}
			}
			dragX = mouseX;
			dragY = mouseY;
			drag = true;
		}
		else
		{
			drag = false;
		}
	}
	GuiContainer::onMouseClick(mouseX, mouseY, button);
}

void GuiWindow::render(int flags, int mouseX, int mouseY)
{
	static CFont* f = CGE::instance->fontRegistry->getCFont(_R("craftgame:futura"));
	if (mouseX >= 0 && mouseY >= 0 && mouseX <= CGE::instance->width && mouseY <= CGE::instance->height)
	{
		if (drag && Keyboard::isKeyDown(Keyboard::LButton))
		{
			x = mouseX - dragX;
			y = mouseY - dragY;
		}
		else
		{
			drag = false;
		}
	}
	Gui::render(flags, mouseX, mouseY);
	CGE::instance->renderer->drawShadow(0, 1, width, height, CGE::instance->fcs == this ? Utils::fromHex(0x378deeff) : glm::vec4(0.f, 0.f, 0.f, 0.3f), 4, 13);
	calculateMask();

	CGE::instance->renderer->setTexturingMode(1);
	CGE::instance->renderer->blur(0, 0, width, height);
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(Utils::fromHex(0x378deeaa));
	CGE::instance->renderer->drawRect(0, 0, width, 35);
	y += 35;
	height -= 35;
	CGE::instance->renderer->transform = getTransform();
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(glm::vec4(0, 0, 0, 0.4));
	CGE::instance->renderer->drawRect(0, 0, width, height);
	GuiContainer::renderWidgets(flags, mouseX, mouseY);
	y -= 35;
	height += 35;
	CGE::instance->renderer->transform = getTransform();
	CGE::instance->maskFB->begin();
	CGE::instance->guiShader = CGE::instance->maskShader;
	CGE::instance->guiShader->start();
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->guiShader = CGE::instance->standartGuiShader;
	CGE::instance->guiShader->start();
	CGE::instance->maskFB->end();
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(Utils::fromHex(0x378deeaa));
	CGE::instance->renderer->drawRect(0, 0, width, 35);
	CGE::instance->renderer->setTexturingMode(1);
	CGE::instance->renderer->drawString(10, 5, title, Align::LEFT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.9), 19, f);
	for (size_t i = 0; i < buttons.size(); i++)
	{
		if (buttons[i]->visibility == Visibility::VISIBLE)
		{
			int rf = 0;
			glm::mat4 f = buttons[i]->getTransform();
			glm::vec4 v = f * glm::vec4(1, 1, 1, 1);

			int x = int((v.x - 1) / 2.f * CGE::instance->width);
			int y = int((v.y - 1) / -2.f * CGE::instance->height);
			if (!(rf & RenderFlags::OVERLAYED) && x <= mouseX && x + buttons[i]->width >= mouseX && y <= mouseY && y + buttons[i]->height >= mouseY)
				rf |= RenderFlags::MOUSE_HOVER;
			int rx = mouseX - x;
			int ry = mouseY - y;
			buttons[i]->render(rf, rx, ry);
			CGE::instance->renderer->transform = getTransform();
			CGE::instance->renderer->color = getColor();
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

void GuiWindow::onLayoutChange(int x, int y, int width, int height)
{
	GuiContainer::onLayoutChange(x, y, width, height);
	size_t w = 3;
	for (size_t i = 0; i < buttons.size(); i++)
	{
		buttons[i]->onLayoutChange(width - buttons[i]->width - w, 0, buttons[i]->width, buttons[i]->height);
		w += buttons[i]->width;
	}
}

void GuiWindow::addTitleButton(Gui* b)
{
	buttons.push_back(b);
	b->parent = this;
}

void GuiWindow::close()
{
	openClose->state = State::inverse;
	openClose->onAnimationEnd = std::function<void(State)>([&](State s)
	{
		if (s == State::inverse)
		{
			CGE::instance->uiThread.push(std::function<void()>([&, s]()
			{
				CGE::instance->removeGuiWindow(this);
			}));
		}
	});
}

std::string& GuiWindow::getClassname()
{
	return classname;
}
#endif
