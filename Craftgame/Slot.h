#pragma once
#include "ItemStack.h"

class Slot
{
public:
#ifndef SERVER
	float highlight = 0.f;
	int width = 52, height = 52;
#endif
	int x, y;
	Slot(int x, int y);
	ItemStack* stack = nullptr;
	virtual bool accepts(ItemStack* s);
};
