#pragma once
#include "Item.h"
#include "ItemStackAnimatable.h"

class ItemAnimator: public ItemW<ItemStackAnimatable>
{
public:
	ItemAnimator(const _R& r, bid id, byte data, IItemRenderer* renderer)
		: ItemW<ItemStackAnimatable>(r, id, data, renderer)
	{
	}

	virtual ~ItemAnimator() = default;
	virtual void animate(ItemStackAnimatable*) {};
};
