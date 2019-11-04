#pragma once

#include "Gui.h"
#include <functional>

class GuiSeekBar : public Gui {
private:
	bool drag;
	int maxValue;
	int prevValue;
	std::string s;
public:
	GuiSeekBar(std::function<std::string(int)> onValueChanged, int maxValue, int value = 0);
	~GuiSeekBar();
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void onMouseClick(int x, int y, int button);
	bool enabled = true;
	int value = 0;
	int minValue = 0;
	std::function<std::string(int)> onValueChanged;
	std::function<void(int)> onValueChangingStop;

	bool isDrag() const;
};