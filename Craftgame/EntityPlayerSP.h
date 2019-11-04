#pragma once

#include "EntityPlayerMP.h"

class EntityPlayerSP: public EntityPlayerMP
{
	private:
	byte prevItemIndex = 0;
public:
	EntityPlayerSP(World* w, const size_t _id, const GameProfile& _g);

	virtual void setFlying(bool b);
	virtual void setLook(float yaw, float pitch);
	virtual void render();
	virtual void tick();
	virtual void setItemIndex(byte index);
	byte getPrevItemIndex();

	float itemAnim = 0.5f;
	void itemAnimation();
};