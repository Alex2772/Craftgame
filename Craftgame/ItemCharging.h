#pragma once
#include "ItemStack.h"

class IStackCharging
{
public:

	IStackCharging() {}
	virtual ~IStackCharging();

	size_t ticksCharging = 0;
	bool isUsed = false;
};

class ItemCharging
{
public:
	ItemCharging() {}
	virtual void tick(ItemStack* stack, EntityPlayerMP* sabj);

	virtual void startCharging(ItemStack* s, EntityPlayerMP* mp);
	virtual void stopCharging(ItemStack* s, EntityPlayerMP* mp);
};
