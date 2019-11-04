#pragma once

#include "GuiScreenDialog.h"
#include "GuiList.h"

class GuiScreenSettings : public GuiScreenDialog {
public:
	GuiScreenSettings();
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void onKeyDown(Keyboard::Key key, byte data);
	virtual void onLayoutChange(int x, int y, int width, int height);
	void refreshMods();
	GuiList* mods;
};
