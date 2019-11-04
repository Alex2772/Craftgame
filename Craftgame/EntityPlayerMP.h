#pragma once

#include "GameProfile.h"
#include "Socket.h"
#include "EntityPlayer.h"
#include "IChunkTracker.h"

class NetPlayerHandler;
class EntityPlayerMP: public EntityPlayer, public IChunkTracker {
private:
	Socket* socket;
	NetPlayerHandler* netHandler;
public:
	glm::vec3 selfMotion;
	EntityPlayerMP(World* w, const size_t& _id, GameProfile _g);
	EntityPlayerMP();
	virtual ~EntityPlayerMP();

	virtual void setPos(const DPos& p) override;
	virtual void setPosNonServer(const DPos& po) override;

	NetPlayerHandler* getNetHandler()
	{
		return netHandler;
	}
	void setNetHandler(NetPlayerHandler* n)
	{
		netHandler = n;
	}
	virtual void setBlock(Block* te, const Pos& p);
	virtual void tick();
	void selectChunks();
	void openInterface(CSInterface* interface);
	virtual void applyMotion(glm::vec3 _motion);
	char state = 0;
	void updateState(char newState);
	ItemStack* itemCharging = nullptr;
	virtual ItemStack* getHeldItem() override;
	virtual bool performCollisionChecks();
};

#include "NetPlayerHandler.h"