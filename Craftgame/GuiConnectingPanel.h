#pragma once

#include "GuiContainer.h"
#include <ctime>

class GuiConnectingPanel : public GuiContainer {
private:
	float animTexture = 0;
	std::time_t timer;
	Animation* animation;
public:
	bool displayTop = true;
	std::string first;
	std::string second;
	GuiConnectingPanel();
	void resetTimer();
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void onLayoutChange(int x, int y, int width, int height);
	virtual void onMouseClick(int mouseX, int mouseY, int button);
	void collapse();
	void show();
};