#pragma once
#include <vector>
#include <string>
#include "GuiScreenDialog.h"
#include "Font.h"

class GuiScreenMessageDialog : public GuiScreenDialog {
protected:
	std::string title;
	std::string orig;
	std::vector<std::string> text;
	Font* f;
	unsigned char* result;
public:
	GuiScreenMessageDialog(std::string title, std::string text, unsigned char* result);
	virtual void render(int rf, int mouseX, int mouseY);
	virtual void close();
	virtual void onLayoutChange(int x, int y, int width, int height);
};