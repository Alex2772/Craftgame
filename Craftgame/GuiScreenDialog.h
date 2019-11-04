#pragma once

#include "GuiScreen.h"
#include "GuiButton.h"

class GuiScreenDialog : public GuiScreen, public IDrawable {
private:
	Animation* animation;
	float highlightAnimation = 0.f;
	std::vector<GuiButton*> buttons;
protected:
	std::string title;
public:
	void addButton(GuiButton* b);
	GuiScreenDialog(Animation* animation);
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void onLayoutChange(int x, int y, int width, int height);
	virtual void close();
	virtual void draw(Gui* widget, int flags);
	virtual void onKeyDown(Keyboard::Key k, byte data);
};
