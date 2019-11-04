#pragma once
#include "InventoryContainer.h"

class EntityPlayerMP;

class InventoryPlayerContainer: public InventoryContainer
{
private:
	EntityPlayerMP* player;
	void slotUpdated(size_t index);
public:
	InventoryPlayerContainer(EntityPlayerMP* p);

	virtual ItemStack* swap(size_t index, ItemStack* tmp) override;
	virtual bool get(size_t index, uint16_t count, ItemStack*& dst) override;
	virtual bool put(size_t index, uint16_t count, ItemStack*& from) override;
};
