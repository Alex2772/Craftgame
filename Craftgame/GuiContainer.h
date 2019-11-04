#pragma once
#include "Gui.h"
#include <vector>
#include "Keyboard.h"
#include <memory>

class GuiContainer : public Gui {
public:
	GuiContainer();
	virtual ~GuiContainer();
	virtual void onMouseClick(int mouseX, int mouseY, int button);
	virtual void render(int flags, int mouseX, int mouseY);
	void drawWidget(Gui* w, glm::mat4& tr, glm::vec4& c, int _rf, int mouseX, int mouseY);
	virtual void renderWidgets(int rf, int mouseX, int mouseY);
	virtual void onKeyDown(Keyboard::Key key, byte data);
	virtual void onLayoutChange(int x, int y, int _width, int _height);
	virtual void addWidget(Gui* gui);
	virtual void addWidget(std::shared_ptr<Gui> gui);
	virtual void onWheel(int mouseX, int mouseY, short d);
	virtual void onText(char c);
	void findFocus(int side);
	typedef std::vector<std::shared_ptr<Gui>> Widgets;
	Widgets widgets;
	Gui* focus = nullptr;
};
