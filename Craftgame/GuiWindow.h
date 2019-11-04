#pragma once
#ifndef SERVER
#include "GuiContainer.h"
#include "Gui.h"
#include <vector>
#include "AnimationDialog.h"

class GuiWindow : public GuiContainer {
protected:
	size_t dragX, dragY;
	bool drag = false;
	std::vector<Gui*> buttons;
	Animation* openClose;
	std::string title;
	std::string classname;
public:
	GuiWindow(std::string title, size_t x, size_t y, size_t width, size_t height);
	GuiWindow(std::string title, size_t width, size_t height);
	virtual void onMouseClick(int mouseX, int mouseY, int button);
	virtual void render(int flags, int mouseX, int mouseY);
	virtual void onLayoutChange(int x, int y, int width, int height);
	virtual void addTitleButton(Gui* b);
	virtual void close();
	virtual std::string& getClassname();
};
#endif