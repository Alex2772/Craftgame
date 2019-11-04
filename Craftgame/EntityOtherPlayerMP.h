#pragma once
#include "EntityPlayerMP.h"

class EntityOtherPlayerMP: public EntityPlayerMP
{
public:
	ItemStack* heldItem = nullptr;
	EntityOtherPlayerMP(World* w, const size_t& _id, const GameProfile& _g)
		: EntityPlayerMP(w, _id, _g)
	{
	}

	EntityOtherPlayerMP()
	{
	}

	virtual ItemStack* getHeldItem() override;
};