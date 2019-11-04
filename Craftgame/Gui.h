#pragma once

#include <functional>
#include <glm/glm.hpp>
#include "Keyboard.h"
#include "global.h"
#include <deque>

class GuiContainer;
class Animation;
class GuiMenu;

enum RenderFlags {
	STANDART = 2,
	MOUSE_HOVER = 4,
	FOCUS = 8,
	OVERLAYED = 16,
	NO_BLUR = 32
};

enum Visibility {
	VISIBLE,
	INVISIBLE,
	GONE
};

class Gui {
protected:
	bool inside(int _x, int _y) {
		return x <= _x && x + width >= _x && y <= _y && y + height >= _y;
	}
	std::string mName;
public:
	GuiMenu* contextMenu = nullptr;
	GuiContainer* parent = nullptr;
	glm::mat4 transform = glm::mat4(1.0);
	glm::vec4 color = glm::vec4(1, 1, 1, 1);
	glm::mat4 animTransform;
	glm::vec4 animColor;
	Gui();
	virtual ~Gui();
	virtual void render(int flags, int mouseX, int mouseY);
	virtual void calculateMask();
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	virtual const std::string& name()
	{
		if (mName.empty()) {
			std::string s = std::string(typeid(*this).name());
			mName = s;// Parsing::splitString(s, " ")[1];
		}
		return mName;
	}
	virtual void onMouseClick(int mouseX, int mouseY, int button);
	std::function<void(int, int, int)> clickListener;
	virtual void onLayoutChange(int x, int y, int width, int height);
	virtual void onWheel(int mouseX, int mouseY, short d);
	virtual void onKeyDown(Keyboard::Key key, byte data);
	virtual void onText(char c);
	virtual bool isFocusable();
	virtual int getMinimumWidth();
	bool isFocused();
	glm::mat4 getTransform();
	glm::vec4 getColor();
	std::deque<Animation*> animations;
	Visibility visibility = Visibility::VISIBLE;
	Gui* tooltip = nullptr;
};
#include "Animation.h"