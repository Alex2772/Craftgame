#include "GuiProgressBar.h"
#ifndef SERVER
#include "AnimationFade.h"
#include "GuiScreenScreenshots.h"
#include "GuiScreenPause.h"
#include "GuiButton.h"
#include "GameEngine.h"
#include "GuiScreenSettings.h"
#include "RemoteServer.h"
#include "GuiSeekBar.h"

//std::shared_ptr<GuiSeekBar> pr;

GuiScreenPause::GuiScreenPause()
{
	addWidget(new GuiButton(_("gui.pause.continue"), [&](int, int, int)
	{
		close();
	}));
	addWidget(new GuiButton(_("gui.mm.settings"), [&](int, int, int)
	{
		CGE::instance->displayGuiScreen(new GuiScreenSettings);
	}));
	addWidget(new GuiButton(_("gui.pause.disconnect"), [&](int, int, int)
	{
		CGE::instance->disconnect();
	}));

	addWidget(RemoteServer::playerList);
	/*
	GuiButton* screenshotLibrary;
	
	addWidget(screenshotLibrary = new GuiButton(_("gui.screenshot_library"), [&](int, int, int)
	{
		CGE::instance->displayGuiScreen(new GuiScreenScreenshots);
	}));
	screenshotLibrary->background = nullptr;
	screenshotLibrary->textAligment = Align::LEFT;
	*/
	/*
	if (!pr.get())
		pr = std::shared_ptr<GuiSeekBar>(new GuiSeekBar([](int k)
	{
		return std::to_string(k);
	}, 1000, 0));
	addWidget(pr); */
	animations.push_back(new AnimationFade(this));
}

void GuiScreenPause::onLayoutChange(int x, int y, int _width, int _height)
{
	GuiScreen::onLayoutChange(x, y, _width, _height);
	widgets[0]->onLayoutChange(20, 100, 400, 40);
	widgets[1]->onLayoutChange(20, 160, 400, 40);
	widgets[2]->onLayoutChange(20, 220, 400, 40);
	widgets[3]->onLayoutChange(width / 2 + 50, 50, width / 2 - 100, height - 100);
	//widgets[4]->onLayoutChange(5, height - 40, 400, 40);
	//pr->onLayoutChange(4, 4, width - 8, 40);
}

void GuiScreenPause::render(int flags, int mouseX, int mouseY)
{
	GuiScreen::render(flags, mouseX, mouseY);
	static CFont* f = CGE::instance->fontRegistry->getCFont(_R("craftgame:futuralight"));
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(glm::vec4(0.f, 0.f, 0.f, 0.4f));
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->renderer->setTexturingMode(3);
	CGE::instance->guiShader->loadVector("colors[0]", glm::vec4(0.f, 0.f, 0.f, 0.7f) * color);
	CGE::instance->guiShader->loadVector("colors[1]", glm::vec4(0.f));
	CGE::instance->guiShader->loadVector("colors[2]", glm::vec4(0.f, 0.f, 0.f, 0.7f) * color);
	CGE::instance->guiShader->loadVector("colors[3]", glm::vec4(0.f));
	CGE::instance->renderer->drawRect(0, 0, width / 3, height);
	CGE::instance->renderer->drawString(20, 20, "CRAFTGAME", Align::LEFT, TextStyle::SIMPLE, glm::vec4(1.f), 40, f);
	//pr->value += (rand() % 1000) / 1000.f;
	//pr->value = fmodf(pr->value, pr->max);
	{
		char buf[64];
		std::time_t t = time(0);
		std::strftime(buf, sizeof(buf), "%H:%M", localtime(&t));
		CGE::instance->renderer->drawString(width - 4, 4, buf, Align::RIGHT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.3) * getColor());
	}
}

void GuiScreenPause::close()
{
	animatedClose(animations[0]);
}
#endif
