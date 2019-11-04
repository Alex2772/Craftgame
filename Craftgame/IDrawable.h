#pragma once

#include "Gui.h"

class IDrawable
{
public:
	virtual void draw(Gui* widget, int flags) = 0;
};