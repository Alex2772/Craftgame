#ifndef SERVER
#include "GuiScreenMessageDialog.h"
#include "GameEngine.h"
#include "AnimationDialog.h"
#include "GuiButton.h"

GuiScreenMessageDialog::GuiScreenMessageDialog(std::string _title, std::string _text, unsigned char* _result):
	GuiScreenDialog(new AnimationDialog(this)),
	title(_title),
	orig(_text),
	result(_result)
{
	addWidget(new GuiButton("OK", std::function<void(int, int, int)>([&](int, int, int) {
		close();
	})));

	f = CGE::instance->fontRegistry->getFont(_R("craftgame:default"));
}
void GuiScreenMessageDialog::close() {
	*result = 0;
	GuiScreenDialog::close();
}
void GuiScreenMessageDialog::render(int rf, int mouseX, int mouseY) {
	GuiScreenDialog::render(rf, mouseX, mouseY);
	glm::vec4 color = getColor();
	CGE::instance->renderer->drawString(16, 16, title, Align::LEFT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.6) * color, 22);
	for (int i = 0; i < text.size(); i++)
		CGE::instance->renderer->drawString(16, 14 * 2 * i + 64, text[i], Align::LEFT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.9) * color);
	CGE::instance->guiShader->loadVector("c", glm::vec4(0.2, 0.2, 0.2, 0.6) * color);
	CGE::instance->guiShader->loadInt("texturing", 0);
	CGE::instance->renderer->drawRect(0, height - 50, width, 50);
}
void GuiScreenMessageDialog::onLayoutChange(int _x, int _y, int _width, int _height) {
	width = 380;
	text = f->trimStringToMultiline(orig, width - 32);
	height = 140 + text.size() * 14 * 2;
	x = (CGE::instance->width - width) / 2;
	y = (CGE::instance->height - height) / 2;
	widgets[0]->onLayoutChange(width - 105, height - 45, 100, 40);
}
#endif