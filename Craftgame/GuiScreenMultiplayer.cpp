#ifndef SERVER
#include "GuiScreenMultiplayer.h"
#include "GuiButton.h"
#include "GameEngine.h"
#include "GuiScreenDirectConnect.h"
#include "AnimationFade.h"

GuiScreenMultiplayer::GuiScreenMultiplayer(): GuiScreenMM(_("gui.multiplayer"))
{
	addWidget(new GuiButton(_("gui.connect"), [](int, int, int)
	{
		//CGE::instance->displayGuiScreen(new GuiScreenDirectConnect);
	}));
	addWidget(new GuiButton(_("gui.directconnect"), [](int, int, int)
                        {
	                        CGE::instance->displayGuiScreen(new GuiScreenDirectConnect);
                        }));
	addWidget(new GuiButton(_("gui.mp.addserver"), [](int, int, int)
	{
		//CGE::instance->displayGuiScreen(new GuiScreenDirectConnect);
	}));
	addWidget(new GuiButton(_("gui.mp.editserver"), [](int, int, int)
	{
		//CGE::instance->displayGuiScreen(new GuiScreenDirectConnect);
	}));
	addWidget(new GuiButton(_("gui.cancel"), [&](int, int, int)
	{
		close();
	}));
	animations.push_back(new AnimationFade(this));
}

void GuiScreenMultiplayer::close()
{
	animatedClose(animations[0]);
}

void onlch(std::shared_ptr<Gui> g, int x, int y, int width, int height)
{
	g->onLayoutChange(x + 2, y + 2, width - 2, height - 2);
}
void GuiScreenMultiplayer::onLayoutChange(int x, int y, int _width, int _height)
{
	GuiScreenMM::onLayoutChange(x, y, _width, _height);
	int cp_w = _width / 3 * 2;
	int cp_x = (_width - cp_w) / 2;
	// Сверху
	{
		onlch(widgets[0], cp_x, height - 40 - 50, cp_w / 2, 40);
		onlch(widgets[1], cp_x + cp_w / 2, height - 40 - 50, cp_w / 2, 40);
	}
	//Снизу
	{
		onlch(widgets[2], cp_x, height - 50, cp_w / 3, 40);
		onlch(widgets[3], cp_x + cp_w / 3, height - 50, cp_w / 3, 40);
		onlch(widgets[4], cp_x + (cp_w / 3) * 2, height - 50, cp_w / 3, 40);
	}
}

void GuiScreenMultiplayer::render(int flags, int mouseX, int mouseY)
{
	GuiScreenMM::render(flags, mouseX, mouseY);

}
#endif