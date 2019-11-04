#include "Slot.h"

Slot::Slot(int _x, int _y):
	x(_x),
	y(_y)
{

}

bool Slot::accepts(ItemStack* s)
{
	return true;
}
