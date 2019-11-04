#pragma once
#include "GuiScreen.h"

#include "gl.h"
#include <glm/glm.hpp>
#include <vector>
#include "LightSource.h"
#include "EntityPlayer.h"

class GuiScreenMainMenu : public GuiScreen {
public:
	GuiScreenMainMenu();
	~GuiScreenMainMenu();
	float posX = 0;
	float posY = 0;
	float posZ = 0;
	float yaw = 0;
	float pitch = 0;
	bool dbg = false;
	LightSource * l = nullptr;
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void onLayoutChange(int x, int y, int width, int height);
	virtual void onKeyDown(Keyboard::Key key, byte data);
	virtual void renderWidgets(int rf, int mouseX, int mouseY);
};