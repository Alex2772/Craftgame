#ifndef SERVER
#include "GuiScreenRegister.h"
#include "AnimationDialog.h"
#include "GameEngine.h"
#include "GuiTextField.h"
#include <boost/algorithm/string.hpp>

GuiScreenRegister::GuiScreenRegister(std::function<void(const std::string&)> f):
	GuiScreenDialog(new AnimationDialog(this))
{
	GuiTextField* t = new GuiTextField();
	t->setText(CGE::instance->settings->getPropertyString("gameprofile/username"));
	addWidget(t);
	addButton(new GuiButton(_("gui.done"), std::function<void(int, int, int)>([&, t, f](int, int, int) {
		std::string s = t->getText();
		boost::erase_all(s, " ");
		boost::erase_all(s, ";");
		boost::erase_all(s, "?");
		boost::erase_all(s, ".");
		boost::erase_all(s, ",");
		if (s.length() > 4) {
			close();
			f(t->getText());
		}
	})));
}

void GuiScreenRegister::render(int rf, int mouseX, int mouseY) {
	GuiScreenDialog::render(rf, mouseX, mouseY);
	glm::vec4 color = getColor();
	CGE::instance->renderer->drawString(width / 2, 16, _("gui.register"), Align::CENTER, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.6) * color, 22);
}
void GuiScreenRegister::onLayoutChange(int _x, int _y, int _width, int _height) {
	GuiScreenDialog::onLayoutChange((_width - 350) / 2, (_height - 160) / 2, 350, 160);
	widgets[0]->onLayoutChange(10, 60, width - 20, 30);
}
#endif