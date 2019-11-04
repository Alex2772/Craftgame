#include "Item.h"
#include "ItemStack.h"

#ifndef SERVER
#include "GameEngine.h"
#endif

Item::Item(_R r, bid i, byte d, IItemRenderer* rr):
	res(r),
	id(i),
	data(d),
	renderer(rr)
{
}

bid& Item::getId()
{
	return id;
}

byte Item::getData()
{
	return data;
}

_R Item::getResource()
{
	return res;
}

uint16_t Item::getMaxStackCount()
{
	return 64;
}

IItemRenderer* Item::getRenderer()
{
	return renderer;
}

ItemStack* Item::newStack()
{
	return new ItemStack(this);
}

int Item::getTF()
{
	return T_ALL;
}

/**
 * \brief Событие левого клика мышью игроком
 * \param sabj Игрок, кликнувший предметом
 * \param stack Предмет
 * \return Передавать событие дальше (блокам, например)
 */
bool Item::itemLeftClick(EntityPlayerMP* sabj, ItemStack* stack)
{
	return true;
}

/**
* \brief Событие правого клика мышью игроком
* \param sabj Игрок, кликнувший предметом
* \param stack Предмет
* \return Передавать событие дальше (блокам, например)
*/
bool Item::itemRightClick(EntityPlayerMP* sabj, ItemStack* stack)
{
#ifndef SERVER
	if (CGE::instance->thePlayer == sabj)
	{
		CGE::instance->thePlayer->itemAnimation();
	}
#endif
	return true;
}
