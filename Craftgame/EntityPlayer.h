#pragma once

#include "GameProfile.h"
#include "EntitySkeleton.h"
#include "InventoryContainer.h"
#include "SkinManager.h"
class CSInterface;
class EntityPlayer: public EntitySkeleton {
private:
	byte itemIndex = 0;
	GameProfile gameProfile;
	size_t* skin = nullptr;
public:
	CSInterface* interface = nullptr;
	ItemStack* tmpInv = nullptr;
	EntityPlayer();
	EntityPlayer(World* w, const size_t& _id, GameProfile& _g);
	virtual ~EntityPlayer();
	const GameProfile& getGameProfile();
	virtual void render();
	virtual void unpack();
	InventoryContainer* inventory = nullptr;
	byte getItemIndex();
	void setItemIndex(byte index);
	virtual ItemStack* getHeldItem();
	virtual void tick() override;
};

#include "CSInterface.h"