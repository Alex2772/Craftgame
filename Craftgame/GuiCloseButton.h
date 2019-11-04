#pragma once
#ifndef SERVER
#include "GameEngine.h"
#include "Gui.h"
#include <functional>

class GuiCloseButton : public Gui {
private:
	GLuint i;
	float animation = 0;
public:
	GuiCloseButton(GLuint i, std::function<void(int, int, int)> click);
	void render(int flags, int mouseX, int mouseY);
};
#endif