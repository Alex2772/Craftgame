#pragma once

class ItemStack;
class Item;

class IItemRenderer
{
public:
	enum Type
	{
		FIRST_PERSON,
		INVENTORY,
		ENTITY,
		DROP
	};
	virtual void render(Type t, Item* item, ItemStack* stack) = 0;
};
