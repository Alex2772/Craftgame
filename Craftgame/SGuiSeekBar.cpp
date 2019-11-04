#ifndef SERVER
#include "SGuiSeekBar.h"
#include "GuiText.h"
#include "GuiSeekBar.h"
#include "GameEngine.h"

extern glm::vec4 globalGuiColor;

SGuiSeekBar::SGuiSeekBar(std::string title, std::string prop, std::function<std::string(int)> m, std::function<void(int)> vcs, int max)
{
	height = 40;
	addWidget(new GuiText(title));
	GuiSeekBar* s = new GuiSeekBar(m, max, CGE::instance->settings->getProperty<int>(prop));
	s->onValueChangingStop = [vcs, prop](int v)
	{
		CGE::instance->settings->setProperty(prop, v);
		CGE::instance->settings->apply();
		vcs(v);
	};
	addWidget(s);
}

SGuiSeekBar::~SGuiSeekBar()
{
	globalGuiColor = glm::vec4(1.f);
}

void SGuiSeekBar::render(int flags, int mouseX, int mouseY)
{

	if (isFocused()) {
		if (std::dynamic_pointer_cast<GuiSeekBar>(widgets[1])->isDrag())
		{
			alpha -= CGE::instance->millis * 6.f;
		}
		else
		{
			alpha += CGE::instance->millis * 2.f;
		}
		globalGuiColor.a = 1.f;
		GuiContainer::render(flags, mouseX, mouseY);
		GuiContainer::renderWidgets(flags, mouseX, mouseY);
		alpha = glm::clamp(alpha, 0.f, 1.f);
		globalGuiColor.a = alpha;
	} else
	{
		GuiContainer::render(flags, mouseX, mouseY);
		GuiContainer::renderWidgets(flags, mouseX, mouseY);
	}
}

void SGuiSeekBar::onLayoutChange(int x, int y, int _width, int _height)
{
	GuiContainer::onLayoutChange(x, y, _width, _height);
	widgets[0]->width = _width / 3 * 2;
	widgets[1]->x = widgets[0]->width;
	widgets[1]->width = _width / 3;
}
#endif