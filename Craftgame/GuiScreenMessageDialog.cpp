#ifndef SERVER
#include "GuiScreenMessageDialog.h"
#include "GameEngine.h"
#include "AnimationDialog.h"
#include "GuiButton.h"
#include "Utils.h"
#include "colors.h"

GuiScreenMessageDialog::GuiScreenMessageDialog(std::string _title, std::string _text):
	GuiScreenMessageDialog(_title, _text, [](int){}, MD_OK)
{
}

GuiScreenMessageDialog::GuiScreenMessageDialog(std::string _title, std::string _text, std::function<void(int)> p, uint8_t d):
	GuiScreenDialog(new AnimationDialog(this)),
	orig(_text),
	proc(p)
{
	title = _title;
	if (d & MD_OK) {
		addButton(new GuiButton(_("gui.ok"), std::function<void(int, int, int)>([&](int, int, int)
		{
			if (proc)
				proc(1);
			close();
		})));
	}
	if (d & MD_CANCEL) {
		addButton(new GuiButton(_("gui.cancel"), std::function<void(int, int, int)>([&](int, int, int)
		{
			if (proc)
				proc(2);
			close();
		})));
	}
	f = CGE::instance->fontRegistry->getCFont(_R("craftgame:default"));
}

GuiScreenMessageDialog::GuiScreenMessageDialog(std::string _title, std::string _text, boost::python::object p, uint8_t t):
	GuiScreenMessageDialog(_title, _text, [p](int t)
{
	boost::python::call<void>(p.ptr(), t);
}, t)
{
}

void GuiScreenMessageDialog::close()
{
	if (proc)
		proc(0);
	GuiScreenDialog::close();
}

void GuiScreenMessageDialog::render(int rf, int mouseX, int mouseY)
{
	GuiScreenDialog::render(rf, mouseX, mouseY);
	glm::vec4 color = getColor();
	if (!title.empty()) {
		for (int i = 0; i < text.size(); i++)
			CGE::instance->renderer->drawString(width / 2, 14 * 2 * i + 64, text[i], Align::CENTER, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.9) * color);
	} else
	{
		for (int i = 0; i < text.size(); i++)
			CGE::instance->renderer->drawString(width / 2, 14 * 2 * i + 54, text[i], Align::CENTER, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.9) * color);
	}
}

void GuiScreenMessageDialog::onLayoutChange(int _x, int _y, int _width, int _height)
{
	width = (glm::max)(380, int(orig.length() + 300) / 5 * 4);
	text = f->trimStringToMultiline(orig, width - 64);
	height = 140 + text.size() * 14 * 2;
	x = (CGE::instance->width - width) / 2;
	y = (CGE::instance->height - height) / 2;
	GuiScreenDialog::onLayoutChange(x, y, width, height);
}

void GuiScreenMessageDialog::setText(std::string s)
{
	orig = s;
	text = f->trimStringToMultiline(orig, width - 64);
}
#endif
