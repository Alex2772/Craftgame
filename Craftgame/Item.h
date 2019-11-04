#pragma once
#include "global.h"
#include "IItemRenderer.h"
#include "def.h"
#include "res.h"

class EntityPlayerMP;

class Item
{
private:
	bid id;
	byte data;
	_R res;
	IItemRenderer* renderer;
public:
	enum TransformFlags
	{
		T_ALL = 255,
		T_INVENTORY = 1
	};

	Item(_R r, bid id, byte data = 0, IItemRenderer* renderer = nullptr);
	virtual ~Item() = default;
	bid& getId();
	byte getData();
	_R getResource();
	virtual uint16_t getMaxStackCount();
	virtual IItemRenderer* getRenderer();
	virtual ItemStack* newStack();
	virtual int getTF();
	
	virtual bool itemLeftClick(EntityPlayerMP* sabj, ItemStack* stack);
	virtual bool itemRightClick(EntityPlayerMP* sabj, ItemStack* stack);
	virtual void tick(ItemStack* stack, EntityPlayerMP* sabj) {}
};
template<class IS>
class ItemW: public Item
{
public:

	ItemW(const _R& r, bid id, byte data, IItemRenderer* renderer)
		: Item(r, id, data, renderer)
	{
	}

	virtual ~ItemW() = default;
	virtual ItemStack* newStack()
	{
		return new IS(this);
	}
};
