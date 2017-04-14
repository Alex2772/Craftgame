#ifndef SERVER


#include "GuiScreenMultiplayer.h"
#include "GameEngine.h"
#include "AnimationDialog.h"
#include "GameEngine.h"
#include "GuiTextField.h"
#include "GuiButton.h"
#include "Socket.h"
#include "Parsing.h"

GuiScreenMultiplayer::GuiScreenMultiplayer():
	GuiScreenDialog(new AnimationDialog(this))
	{
	GuiTextField* t = new GuiTextField();
	addWidget(t);
	addWidget(new GuiButton("Connect", std::function<void(int, int, int)>([&, t](int, int, int) {
		if (!t->getText().size())
			t->setText("localhost");
		CGE::instance->threadPool.runAsync([&, t]() {
			std::string j = t->getText();
			std::vector<std::string> s = Parsing::splitString(j, ":");
			if (s.size() == 2) {
				CGE::instance->connectToServer(s[0], stoi(s[1].c_str()));
			}
			else {
				CGE::instance->connectToServer(j, 24565);
			}
			close();
		});
	})));
	focus = widgets[0];
}

void GuiScreenMultiplayer::render(int rf, int mouseX, int mouseY) {
	GuiScreenDialog::render(rf, mouseX, mouseY);
	glm::vec4 color = getColor();
	CGE::instance->renderer->drawString(16, 16, "Connect", Align::LEFT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.6) * color, 22);
}
void GuiScreenMultiplayer::onLayoutChange(int _x, int _y, int _width, int _height) {
	width = 350;
	height = 160;
	x = (_width - 350) / 2;
	y = (_height - 160) / 2;
	widgets[0]->onLayoutChange(10, 60, width - 20, 30);
	widgets[1]->onLayoutChange(width - 150, height - 40, 150, 40);
}

void GuiScreenMultiplayer::onKeyDown(Keyboard::Key key, byte data) {
	GuiScreenDialog::onKeyDown(key, data);
	if (key == Keyboard::Key::ESCAPE) {
		close();
	}
}
#endif