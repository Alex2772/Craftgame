#include "GuiScreenSingleplayer.h"
#include "AnimationFade.h"
#include "GuiButton.h"
#include "GameEngine.h"
#include "GuiScreenDialog.h"
#include "GuiList.h"
#include "GuiText.h"
#include "GuiTextField.h"
#include "GuiLabel.h"

class GuiScreenNewWorld: public GuiScreenDialog
{
private:
	GuiTextField* seed;
	GuiText* seedNotify = new GuiText("");

	long getSeed()
	{
		try
		{
			return std::stol(seed->getText());
		} catch (...)
		{
			return std::hash<std::string>()(seed->getText());
		}
	}

public:
	GuiScreenNewWorld():
	GuiScreenDialog(new AnimationDialog(this))
	{
		title = _("gui.singleplayer.create");
		GuiList* l = new GuiList;

		{
			l->addWidget(new GuiLabel(_("gui.singleplayer.name")));
			GuiTextField* g = new GuiTextField();
			g->setText(_("gui.singleplayer.name.placeholder"));
			l->addWidget(g);
			l->focus = g;
			//Gui* x = new Gui();
			//l->addWidget(x);
			l->addWidget(new GuiLabel(_("gui.singleplayer.seed")));
			seed = new GuiTextField();
			seed->setText(std::to_string(std::time(nullptr)));
			l->addWidget(seed);
			l->addWidget(seedNotify);
		}
		addWidget(l);
		


		addButton(new GuiButton(_("gui.singleplayer.create"), [](int, int, int)
		{

		}));
		addButton(new GuiButton(_("gui.cancel"), [&](int, int, int)
		{
			close();
		}));
	}

	virtual void render(int rf, int mouseX, int mouseY) override {
		std::string st = std::to_string(getSeed());
		if (st != seed->getText())
		{
			seedNotify->visibility = VISIBLE;
			seedNotify->setText(_P("gui.singleplayer.seed_notise", spair("seed", st)));
		} else
		{

			seedNotify->visibility = GONE;
		}
		GuiScreenDialog::render(rf, mouseX, mouseY);
	}

	virtual void onLayoutChange(int x, int y, int width, int height) override {
		width = 600;
		height = 400;
		widgets[0]->onLayoutChange(8, 40, width - 16, height);

		auto pt = std::dynamic_pointer_cast<GuiList>(widgets[0]);

		for (size_t i = 0; i < pt->widgets.size(); ++i)
		{
			pt->widgets[i]->height = 30;
		}
		GuiScreenDialog::onLayoutChange((CGE::instance->width - width) / 2, (CGE::instance->height - height) / 2, width, height);
	}
};

GuiScreenSingleplayer::GuiScreenSingleplayer(): 
GuiScreenMM("gui.singleplayer")
{
	addWidget(new GuiButton(_("gui.singleplayer.play"), [](int, int, int)
	{
		//CGE::instance->displayGuiScreen(new GuiScreenDirectConnect);
	}));
	addWidget(new GuiButton(_("gui.singleplayer.create"), [](int, int, int)
	{
		CGE::instance->displayGuiScreen(new GuiScreenNewWorld);
	}));
	addWidget(new GuiButton(_("gui.singleplayer.edit"), [](int, int, int)
	{
		//CGE::instance->displayGuiScreen(new GuiScreenDirectConnect);
	}));
	addWidget(new GuiButton(_("gui.singleplayer.delete"), [](int, int, int)
	{
		//CGE::instance->displayGuiScreen(new GuiScreenDirectConnect);
	}));
	addWidget(new GuiButton(_("gui.cancel"), [&](int, int, int)
	{
		close();
	}));
	animations.push_back(new AnimationFade(this));
}

void GuiScreenSingleplayer::close()
{
	animatedClose(animations[0]);
}

void onlch(std::shared_ptr<Gui> g, int x, int y, int width, int height);
void GuiScreenSingleplayer::onLayoutChange(int x, int y, int _width, int _height)
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

void GuiScreenSingleplayer::render(int flags, int mouseX, int mouseY)
{
	GuiScreenMM::render(flags, mouseX, mouseY);
}
