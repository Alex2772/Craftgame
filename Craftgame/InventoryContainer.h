#pragma once
#include "Slot.h"

class InventoryContainer
{
protected:
	ItemStack* _swap(size_t index, ItemStack* stack);
public:
	std::vector<Slot*> slots;

	~InventoryContainer();

	virtual ItemStack* swap(size_t index, ItemStack* tmp);
	virtual bool get(size_t index, uint16_t count, ItemStack*& dst);
	virtual bool put(size_t index, uint16_t count, ItemStack*& from);
};
