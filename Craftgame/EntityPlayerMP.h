#pragma once

#include "GameProfile.h"
#include "Socket.h"
#include "EntityPlayer.h"

class EntityPlayerMP: public EntityPlayer {
private:
	Socket* socket;
public:
	EntityPlayerMP(World* w, const size_t& _id, GameProfile _g, Socket* _s) :
		EntityPlayer(w, _id, _g),
		socket(_s)
		{

	}
	virtual Socket* getSocket() {
		return socket;
	}
};
