#pragma once
#include "Gui.h"
#include <vector>
#include "Keyboard.h"

class GuiContainer : public Gui {
public:
	GuiContainer();
	virtual ~GuiContainer();
	virtual void onMouseClick(int mouseX, int mouseY, int button);
	virtual void render(int flags, int mouseX, int mouseY);
	virtual void renderWidgets(int rf, int mouseX, int mouseY);
	virtual void onKeyDown(Keyboard::Key key, byte data);
	virtual void onLayoutChange(int x, int y, int _width, int _height);
	virtual void addWidget(Gui* gui);
	virtual void onWheel(int mouseX, int mouseY, short d);
	virtual void onText(char c);
	std::vector<Gui*> widgets;
	Gui* focus = nullptr;
};