#pragma once

#include "GameProfile.h"
#include "Socket.h"
#include "Entity.h"

class EntityPlayer: public Entity {
private:
	GameProfile gameProfile;
public:
	EntityPlayer(World* w, const size_t& _id, GameProfile& _g) :
		gameProfile(_g),
		Entity(w, _id)
	{

	}
	virtual GameProfile& getGameProfile() {
		return gameProfile;
	}
};