#pragma once
#ifndef SERVER
#include "GuiContainer.h"
#include <vector>
#include <memory>
#include "Keyboard.h"
#include "global.h"

class GuiScreen : public GuiContainer {
public:
	GuiScreen();
	virtual ~GuiScreen();
	virtual void onMouseClick(int mouseX, int mouseY, int button);
	virtual void render(int flags, int mouseX, int mouseY);
	virtual void renderWidgets(int rf, int mouseX, int mouseY);
	virtual void onKeyDown(Keyboard::Key key, byte data);
	virtual void close();
	virtual void animatedClose(Animation* animation);
};

#endif
