#pragma once

#include "GuiContainer.h"

class SGuiSeekBar : public GuiContainer {
private:
	float alpha = 1.f;
public:
	SGuiSeekBar(std::string title, std::string prop, std::function<std::string(int)> m, std::function<void(int)> vcs, int max);
	~SGuiSeekBar();

	virtual void render(int flags, int mouseX, int mouseY) override;
	virtual void onLayoutChange(int x, int y, int _width, int _height) override;
};