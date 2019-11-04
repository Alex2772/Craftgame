#ifndef SERVER


#include "GuiScreenDirectConnect.h"
#include "GameEngine.h"
#include "AnimationDialog.h"
#include "GameEngine.h"
#include "GuiTextField.h"
#include "GuiButton.h"
#include "Socket.h"
#include "Parsing.h"
#include "GameEngine.h"
#include <boost/algorithm/string.hpp>

GuiScreenDirectConnect::GuiScreenDirectConnect():
	GuiScreenDialog(new AnimationDialog(this))
	{
	GuiTextField* t = new GuiTextField();
	t->setText(CGE::instance->settings->getPropertyString("multiplayer/lastserver"));
	addWidget(t);
	addButton(new GuiButton(_("gui.multiplayer.connect"), std::function<void(int, int, int)>([&, t](int, int, int) {
		std::string j = t->getText();
		boost::erase_all(j, " ");
		if (!j.size())
			t->setText("127.0.0.1");
		CGE::instance->threadPool.runAsync([&, j]() {
			CGE::instance->settings->setProperty("multiplayer/lastserver", j);
			CGE::instance->settings->apply();
			std::vector<std::string> s = Parsing::splitString(j, ":");
			if (s.size() == 2) {
				CGE::instance->connectToServer(s[0], std::stoi(s[1].c_str()));
			}
			else {
				CGE::instance->connectToServer(j, 24565);
			}
			close();
		});
	})));
	addButton(new GuiButton(_("gui.cancel"), std::function<void(int, int, int)>([&, t](int, int, int) {
		close();
	})));
	focus = widgets[0].get();
}

void GuiScreenDirectConnect::render(int rf, int mouseX, int mouseY) {
	GuiScreenDialog::render(rf, mouseX, mouseY);
	glm::vec4 color = getColor();
	CGE::instance->renderer->drawString(width / 2, 16, _("gui.directconnect"), Align::CENTER, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.6) * color, 22);
}
void GuiScreenDirectConnect::onLayoutChange(int _x, int _y, int _width, int _height) {
	GuiScreenDialog::onLayoutChange((_width - 350) / 2, (_height - 160) / 2, 350, 160);
	widgets[0]->onLayoutChange(10, 60, width - 20, 30);
}

void GuiScreenDirectConnect::onKeyDown(Keyboard::Key key, byte data) {
	GuiScreenDialog::onKeyDown(key, data);
	switch (key) {
	case Keyboard::Escape:
		close();
		break;
	} 
}
#endif