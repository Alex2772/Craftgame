#pragma once
#include "GuiScreen.h"
#include "GuiTextField.h"
#include "P03ChatMessage.h"
#include "GameEngine.h"
#include "AnimationFade.h"
class CustomTextField : public GuiTextField {
public:
	bool first = true;

	CustomTextField()
	{
	}

	virtual void onText(char c) override {
		if (first)
		{
			first = false;
		}
		else
		{
			GuiTextField::onText(c);
		}
	}
};

class GuiScreenChat: public GuiScreen
{
private:
	Animation* animation;
	size_t chatHistoryIndex = 0;
	std::string storedString;
	std::vector<Command*> cmds;
	std::vector<std::string> autocomplete;
	size_t index = 0;
	size_t autocompleteIndex = 0;
	bool autocompleteMode = false;
public:
	CustomTextField* f;
	GuiScreenChat();
	~GuiScreenChat();

	virtual void onWheel(int mouseX, int mouseY, short d) override {
		GuiScreen::onWheel(mouseX, mouseY, d);
		if (CGE::instance->chat->scroll + d < 0)
		{
			CGE::instance->chat->scroll = 0;
		} else if (CGE::instance->chat->height <= 186 || d < 0)
		{
			CGE::instance->chat->scroll += d;
		}
	}

	virtual void onLayoutChange(int x, int y, int _width, int _height) override {
		GuiScreen::onLayoutChange(x, y, _width, _height);
		f->onLayoutChange(4, height - 30, width - 12, 30);
	}

	virtual void render(int flags, int mouseX, int mouseY);

	virtual void onKeyDown(Keyboard::Key key, byte data) override;
	void autocompleteNext();
	void autocompleteReceived(const std::vector<std::string>& a);
};
