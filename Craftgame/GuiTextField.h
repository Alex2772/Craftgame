#pragma once
#include "Gui.h"
#include "Keyboard.h"
#include "global.h"

class GuiTextField : public Gui {
private:
	std::string text;
	bool enabled = true;
public:
	GuiTextField();
	size_t cursor = 0;
	size_t scroll = 0;
	long textsize = 13;
	int selection = -1;
	float blink = 0;
	std::string getText();
	std::string getSelectedText();
	void setText(std::string s);
	void setEnabled(bool enabled);
	virtual bool isEnabled();
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void onKeyDown(Keyboard::Key key, byte data);
	virtual void onText(char c);
	virtual void onMouseClick(int mX, int mY, int button);
};