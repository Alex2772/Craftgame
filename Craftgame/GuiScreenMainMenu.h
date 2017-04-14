#pragma once
#include "GuiScreen.h"
#include <GL/glew.h>
#include <GL\GL.h>
#include <glm/glm.hpp>
#include <vector>
#include "LightSource.h"

class GuiScreenMainMenu : public GuiScreen {
public:
	GuiScreenMainMenu();
	~GuiScreenMainMenu();
	LightSource * l = nullptr;
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void onLayoutChange(int x, int y, int width, int height);
};