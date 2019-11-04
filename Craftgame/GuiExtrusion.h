#pragma once
#include "Gui.h"
#include "TextureAtlas.h"

class GuiExtrusion: public Gui
{
public:
	enum Side
	{
		TOP_LEFT = 0,
		TOP = 1,
		TOP_RIGHT = 2,
		LEFT = 3,
		CENTER = 4,
		RIGHT = 5,
		BOTTOM_LEFT = 6,
		BOTTOM = 7,
		BOTTOM_RIGHT = 8
	};
	Side top_left = TOP_LEFT;
	Side top = TOP;
	Side top_right = TOP_RIGHT;
	Side left = LEFT;
	Side center = CENTER;
	Side right = RIGHT;
	Side bottom_left = BOTTOM_LEFT;
	Side bottom = BOTTOM;
	Side bottom_right = BOTTOM_RIGHT;
	TextureAtlas atlas;
	GuiExtrusion();
	virtual void render(int flags, int mouseX, int mouseY) override;
};
